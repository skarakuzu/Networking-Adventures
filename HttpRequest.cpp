#include "HTTPRequest.hpp"

void HTTPRequest::parse_body(std::string &line,
                             std::map<std::string, std::string> &body_map) {
  int pos, ref_pos, end_pos, dummy;
  std::string str, first, second;

  ref_pos = 0;
  while (line.find('=', ref_pos) != std::string::npos) {
    pos = line.find('=', ref_pos);
    if (pos != std::string::npos) {
      dummy = line.find('&', ref_pos);
      end_pos = (dummy == std::string::npos ? line.length() : dummy);
      first = line.substr(ref_pos, pos - ref_pos);
      second = line.substr(pos + 1, end_pos - pos - 1);
      body_map.insert({first, second});
      ref_pos = end_pos + 1;
    } else
      break;
  }
}

void HTTPRequest::parser(std::string &request_str) {
  // std::map<std::string, std::string> request_map;
  // std::map<std::string, std::string> header_map;
  // std::map<std::string, std::string> body_map;

  // std::stringstream ss(request_str);
  ss.str(request_str);

  int pos, pos_dot, end_pos, dummy;
  std::string str, line, first, second, version, method, url;

  ss >> method >> line >> version;
  request_map.insert({"method", method});
  request_map.insert({"version", version});

  if (line.length() <= 2 && method == "GET")
    line = "/index.html";

  pos = line.find_first_of('?');
  pos_dot = line.find_first_of('.');
  if (pos != std::string::npos) {
    url = line.substr(0, pos);
    str = line.substr(pos + 1);
    parse_body(str, body_map);

    if (method == "GET")
      fileExtension =
          pos_dot != std::string::npos ? url.substr(pos_dot + 1) : "html";
    if (pos_dot != std::string::npos)
      request_map.insert({"url", url});
    else
      request_map.insert({"url", url + "/index." + fileExtension});
  } else {
    if (method == "GET")
      fileExtension =
          pos_dot != std::string::npos ? line.substr(pos_dot + 1) : "html";
    if (pos_dot != std::string::npos)
      request_map.insert({"url", line});
    else
      request_map.insert({"url", line + "/index." + fileExtension});
  }

  // std::cout<<"The file extension is: "<<fileExtension<<std::endl;

  // std::cout<<"reading ...: "<<method<<" "<<url<<" "<<version<<std::endl;
  // std::cout<<"********* started printing *******************\n";
  std::getline(ss, line); // get rid of the request line
  while (std::getline(ss, line)) {

    // if(line.length() == 1) break;
    if (line.length() <= 1)
      break;
    // std::cout<<"Here with the line: "<<line<<std::endl;
    pos = line.find_first_of(':');
    if (pos != std::string::npos) {
      first = line.substr(0, pos);
      second = line.substr(pos + 2);

      header_map.insert({first, second});
    }
  }

  if (header_map["Content-Type"].find("application/x-www-form-urlencoded") !=
      std::string::npos) {
    // expecting data in the form MyVariableOne=ValueOne&MyVariableTwo=ValueTwo
    while (std::getline(ss, line)) {
      // std::cout<<"Continue Here with the line: "<<line<<std::endl;
      parse_body(line, body_map);
    }
  } else if (header_map["Content-Type"].find("multipart/form-data") !=
             std::string::npos) {
    // std::streampos index_begin = ss.tellg();
    // std::cout<<"the request string start 0 :
    // "<<request_str[index_begin]<<std::endl;

    std::getline(ss, line); // the line that includes boundry
    file_boundry = line.substr(2);
    // std::cout<<"boundry "<<file_boundry<<std::endl;

    std::getline(ss, line);
    // std::cout<<"the line "<<line<<std::endl;
    size_t pos = line.find("filename="); // "filename=" has 10 characters and
                                         // start reading from there
    filename_to_write = line.substr(
        pos + 10, line.size() - 2 - pos -
                      10); // get rid of " beginning and end of the filename
    filename_to_write = '/' + filename_to_write;
    // std::cout<<"the filename : "<<filename_to_write<<std::endl;

    std::getline(ss, line); // get rid of the empty line

    // std::streampos index = ss.tellg();
    // std::cout<<"the request string start 1 :
    // "<<request_str[index]<<std::endl;
  } else if (header_map["Content-Type"].find("application/octet-stream") !=
             std::string::npos) {
    std::getline(ss, line); // get rid of the empty line
    filename_to_write = get_url();
    // std::cout<<"the filename : "<<filename_to_write<<std::endl;
    file_boundry = "";
  }

  /*
  std::cout<<"printing request map..\n";
  for (const auto& pair : request_map)
      std::cout << pair.first << " " << pair.second << std::endl;
  */
  /*
  std::cout<<"printing header map..\n";
  for (const auto& pair : header_map)
      std::cout << pair.first << " " << pair.second << std::endl;
  */
  /*
  std::cout<<"printing body map..\n";
  for (const auto& pair : body_map)
      std::cout << pair.first << " " << pair.second << std::endl;
  std::cout<<"*********** ended printing ****************\n";
  */
}

void HTTPRequest::respond_type_post(int client_socket_fd) {}
void HTTPRequest::respond_type_put(int client_socket_fd) {}

void HTTPRequest::respond(int socket_id, std::string &&buffer,
                          std::unique_ptr<BaseTask> &basePtr) {
  if (request_map["method"] == "GET") {
    std::string content_type = get_content_type();
    basePtr = std::make_unique<ReaderTask>(
        ReaderTask(socket_id, 0, 0, 0, std::forward<std::string>(buffer),
                   get_url(), content_type, " "));

  } else if (request_map["method"] == "POST") {
    basePtr = std::make_unique<WriterTask>(
        WriterTask(socket_id, 0, get_buffer_postion(),
                   std::stoi(header_map["Content-Length"]),
                   std::forward<std::string>(buffer), filename_to_write, " ",
                   file_boundry));
  } else if (request_map["method"] == "PUT") {
    basePtr = std::make_unique<WriterTask>(
        WriterTask(socket_id, 0, get_buffer_postion(),
                   std::stoi(header_map["Content-Length"]),
                   std::forward<std::string>(buffer), filename_to_write, " ",
                   file_boundry));
  }
}

int HTTPRequest::get_buffer_postion() { return int(ss.tellg()); }

std::string HTTPRequest::get_url() { return request_map["url"]; }
std::string HTTPRequest::get_method() { return request_map["method"]; }

std::string HTTPRequest::get_fileExtension() { return fileExtension; }

std::string HTTPRequest::get_content_type() {
  auto it =
      std::find_if(mimetype.begin(), mimetype.end(),
                   [this](const auto &p) { return p.first == fileExtension; });
  return it != mimetype.end() ? std::string(it->second) : nullptr;
}
