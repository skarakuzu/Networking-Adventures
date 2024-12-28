#include "HTTPRequest.hpp"


void HTTPRequest::parser()
{
    std::stringstream ss(request_str);

    int num1;
    double num2;
    std::string str, line, first, second; 

    ss >> method >> str >> url;
    //std::getline(ss, line);

    std::cout<<"reading ...: "<<method<<" "<<str<<" "<<url<<std::endl;
    std::cout<<"********* started printing *******************\n";

    while (std::getline(ss, line)) {
        int pos = line.find_first_of(':');
        if (pos != std::string::npos)
        {
        first= line.substr(0, pos);
        second = line.substr(pos+2);

        request_map.insert({first, second});
        }
   
        int ref_pos=0;
        while(line.find('=', ref_pos) != std::string::npos)
        {
            int pos = line.find('=', ref_pos);
            if (pos != std::string::npos)
            {
            int dummy = line.find('&', ref_pos);
            int end_pos = (dummy == std::string::npos ? line.length() : dummy);
            first= line.substr(ref_pos, pos-ref_pos);
            second = line.substr(pos+1, end_pos-pos-1);
            body_map.insert({first, second});
            //std::cout<<"hey there: "<<first<<" "<<second<<" "<<ref_pos<<" "<<pos<<" "<<dummy<<" "<<end_pos<<" "<<line[11]<<std::endl;
            ref_pos = end_pos +1;
            }
            else 
                break;

        }
        //std::cerr << first<<" : "<< second<<" "<<pos<<" "<<line.length()<<std::endl;
        //std::cout<<"the line--->> "<<line<<std::endl;

    }

    for (const auto& pair : request_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    std::cout<<"printing bosy map..\n";
    for (const auto& pair : body_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    std::cout<<"*********** ended printing ****************\n";
}

