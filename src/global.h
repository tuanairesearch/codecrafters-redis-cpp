//
// Created by tuan on 3/27/26.
//

#ifndef REDIS_STARTER_CPP_GLOBAL_H
#define REDIS_STARTER_CPP_GLOBAL_H
#include <chrono>
#include <deque>
#include <unordered_map>
#include <string>
#include <map>
#include <vector>

struct data {
    std::string value;
    bool has_expired;
    std::chrono::steady_clock::time_point expired_time;
};

struct client_time_data {
    int client_fd;
    bool has_expired;
    double expired_duration;
    std::chrono::steady_clock::time_point expired_time;
};

struct StreamID {
    uint64_t stream_id;
    uint64_t sequence_number = 0;
    StreamID() {
        auto time_now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch()).count();
        stream_id = ms;
    }
    StreamID(uint64_t stream_id) {
        this->stream_id = stream_id;
    }
    StreamID(uint64_t stream_id, uint64_t sequence_number) {
        this->stream_id = stream_id;
        this->sequence_number = sequence_number;
    }
    std::string to_str() const
    {
        return std::to_string(stream_id) + "-" + std::to_string(sequence_number);
    }
    bool operator<(const StreamID &other) const{
        if (stream_id != other.stream_id) return stream_id < other.stream_id;
        return sequence_number < other.sequence_number;
    }
};

struct stream_cilent_blocked_element
{
    int client_fd;
    StreamID stream_id;
    std::chrono::steady_clock::time_point expired_time;
};


// This is for all type of data list, stream,...
struct blocked_client
{
    // TYPE desciption
    // type = 0 : list data
    // type = 1 : stream data

    int client_fd;
    size_t type;
    std::chrono::steady_clock::time_point expired_time;

    // If type = 1, there is addition property
    StreamID stream_id;

};
extern std::unordered_map<std::string, std::map<StreamID,std::vector<std::pair<std::string, std::string>>>> stream_data;

extern std::deque<client_time_data> blocked_clients;

extern  std::deque<stream_cilent_blocked_element> stream_blocked_clients;

extern  std::deque<blocked_client> blocked_clients2;

extern std::unordered_map<std::string, data> client_data_string;

extern std::unordered_map<std::string,std::deque<std::string>> client_data_list;

extern std::deque<client_time_data> client_expired_time;

extern blocked_client nearest_expired();
extern timeval change_time_to_timeval(blocked_client &data);
#endif //REDIS_STARTER_CPP_GLOBAL_H
