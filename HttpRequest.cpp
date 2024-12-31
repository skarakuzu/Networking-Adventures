#include "HTTPRequest.hpp"

void HTTPRequest::parse_body(std::string& line, std::map<std::string, std::string>& body_map)
{    
     int pos, ref_pos, end_pos, dummy;
     std::string str, first, second; 

        ref_pos=0;
        while(line.find('=', ref_pos) != std::string::npos)
        {
            pos = line.find('=', ref_pos);
            if (pos != std::string::npos)
            {
            dummy = line.find('&', ref_pos);
            end_pos = (dummy == std::string::npos ? line.length() : dummy);
            first= line.substr(ref_pos, pos-ref_pos);
            second = line.substr(pos+1, end_pos-pos-1);
            body_map.insert({first, second});
            ref_pos = end_pos +1;
            }
            else 
                break;

        }
}

void HTTPRequest::parser(std::string& request_str)
{
    //std::map<std::string, std::string> request_map;
    //std::map<std::string, std::string> header_map;
    //std::map<std::string, std::string> body_map;

    std::stringstream ss(request_str);

    int pos, pos_dot, end_pos, dummy;
    std::string str, line, first, second, version, method, url; 

    ss >> method >> line >> version;
    request_map.insert({"method", method});
    request_map.insert({"version", version});

    if (line.length() <= 2) line = "/index.html";
    
    pos = line.find_first_of('?');
    pos_dot = line.find_first_of('.');
    if(pos != std::string::npos)
    {
        url = line.substr(0, pos);
        str = line.substr(pos+1);
        parse_body(str, body_map);

        fileExtension = pos_dot != std::string::npos ? url.substr(pos_dot+1) : "html";
        if (pos_dot != std::string::npos) request_map.insert({"url", url});
        else request_map.insert({"url", url + '.' + fileExtension});
    }
    else
    {
        fileExtension = pos_dot != std::string::npos ? line.substr(pos_dot+1) : "html";
        if (pos_dot != std::string::npos) request_map.insert({"url", line});
        else request_map.insert({"url", line + '.' + fileExtension});
    }
    
    //std::cout<<"The file extension is: "<<fileExtension<<std::endl;

    //std::cout<<"reading ...: "<<method<<" "<<url<<" "<<version<<std::endl;
    //std::cout<<"********* started printing *******************\n";
    std::getline(ss, line); //get rid of the request line
    while (std::getline(ss, line)) 
    {

        if(line.length() == 1) break;
        //std::cout<<"Here with the line: "<<line<<std::endl;
        pos = line.find_first_of(':');
        if (pos != std::string::npos)
        {
        first= line.substr(0, pos);
        second = line.substr(pos+2);

        header_map.insert({first, second});
        }

    }

    while (std::getline(ss, line)) 
    {
        //std::cout<<"Here with the line: "<<line<<std::endl;
        parse_body(line, body_map);
    }

    /*
    std::cout<<"printing request map..\n";
    for (const auto& pair : request_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    
    std::cout<<"printing header map..\n";
    for (const auto& pair : header_map)
        std::cout << pair.first << " " << pair.second << std::endl;

    std::cout<<"printing body map..\n";
    for (const auto& pair : body_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    std::cout<<"*********** ended printing ****************\n";
    */
}

std::string HTTPRequest::get_url()
{
    return request_map["url"];
}

std::string HTTPRequest::get_fileExtension()
{
    return fileExtension;
}


std::string HTTPRequest::get_content_type()
{
       auto it = std::find_if(mimetype.begin(), mimetype.end(), [this](const auto& p) { return p.first == fileExtension; });
       return it != mimetype.end() ? std::string(it->second) : nullptr;
}
