#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>

#include "Client.h"
#include "lib/Message.h"
#include "lib/Utils.h"
#include "spdlog/spdlog.h"

int main(int argc, char** argv) {
    spdlog::set_pattern("[%@ %H:%M:%S:%e %z] [%^%L%$] [thread %t] %v");
    int opt;
    int option_index = 0;
    static struct option long_options[] = {{"local_server", required_argument, NULL, 'l'},
                                            {"cproxy_server", required_argument, NULL, 's'},
                                            {"work_thread_nums", required_argument, NULL, 't'},
                                            //  longopts的最后一个元素必须是全0填充，否则会报段错误
                                            {0, 0, 0, 0}};
    std::string local_server_host;
    uint32_t local_server_port;
    std::string cproxy_server_host;
    u_int32_t cproxy_server_port;

    int work_thread_nums = 1;
    while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'l':
            parse_host_port(optarg, local_server_host, local_server_port);
            break;
        case 's':
            parse_host_port(optarg, cproxy_server_host, cproxy_server_port);
            break;
        case 't':
            work_thread_nums = atoi(optarg);
            break;
        default:
            break;
        }
    }

    if (local_server_host.empty() || local_server_port == 0) {
        printf("local_server参数设置错误\n");
        abort();
    };

    if (cproxy_server_host.empty() || cproxy_server_port == 0) {
        printf("cproxy_server参数设置错误\n");
        abort();
    };

     work_thread_nums = work_thread_nums > 0 ? work_thread_nums : 1;
    
    // 主线程loop：处理ctlConn的读写
    // eventloop线程池：处理proxyConn的读写+innerConn的读写
    Client client(work_thread_nums, cproxy_server_host, cproxy_server_port, local_server_host,local_server_port);
    client.Run();
    return 0;
}


