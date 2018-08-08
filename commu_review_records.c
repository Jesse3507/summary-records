route_ext() -> basedate_file_extr() -> basedata_parse()

static ROUTE_EXTR_RESULT basedata_parse(
	struct rt_extrRoadStationTime *train_sch,
	FILE *bd_file, 
	ROUTE_LIST *r_list, 
	STATION_ADDR_T *ss_addr, 
	STATION_ADDR_T *es_addr, 
	uint16_t limit_level);

参数传入：	
*train_sch, 列车时刻表的提取结果，在route_ext()中初始化
*bd_file, basedata.dat的文件流
*r_list, 在route_ext()中申请内存，调用basedate_file_extr()时传入，调用完后再
    route_ext()中free掉

-------------------------------------------------------------------------------
交路提取review记录：
1.limit_merge() -> limit_code_table_extr()；
    limit_code_table_extr()在route_extract.c中定义，定义类型为指针数组
2.limit_merge()函数中，if(cnt_node->value.tele->t_num == 1186)是啥意思；
3.limit_adjust()函数定义前的宏定义：#define AIRWALKDIS 384；
4.limit_adjust()函数里的宏定义：#define spli_dist 2500；
5.limit_merge()函数的参数reve_info在该函数中并没有用到，当时为什么要定义这个参
    数，后续有无作用，没有的话可以将其删除；

-------------------------------------------------------------------------------
通信板程序现状:
    1. 始发站为中间车站时，LKJ要先按开车键后，才会显示优化曲线；
    2. 下行支线3转移测试：后按开车键，支线转移后相应的优化没有出来，不知和后按开
        车键有无关系

-------------------------------------------------------------------------------
函数功能记录
    get_transport_type(): 获取本补客货类型
    set_transport_type(): 获取本补客货类型
    route_extr()：线路数据提取，在main()中调用，在主线程执行，没有开新的线程。在
        揭示信息核对状态JIESHI_INFO_CHECK_STATUS时执行，分上下行调用两次。调用前
        先判断上下行，再判断LKJ的起始车站号是否属于上下行线路，如果属于才会调用
        route_extr()，如果不属于，则置状态机为SYSTEM_ERROR_STATUS
    station_in_num26_route_up(): 判断LKJ输入的起始车站号是否是上行线路的车站
    station_in_num26_route_down(): 判断LKJ输入的起始车站号是否是下行线路的车站
    is_extracted():判断是否已经提取过交路数据
    convert_seq_mile(): 非连续公里标转换成连续公里标
    convert_deadzone(): 转换通信板上文件夹RawData里的dead_zone文件保存到文件夹
        route_data中以发送给核心板和显示器；
    create_cm_node()保存连续公里标
    led_run_blink()状态灯指示函数，在心跳线程heartbeat_thread里调用；
    led_rt_blink()实时信息接收指示函数，用A2灯指示，在set_realtime_info()调用调
        用；
    receive_base_data()比较本地基础数据版本与状态信息里的版本，不一致时发送基础
        数据请求

/*
 *@brief 判断是否已经提取过交路数据
 *@param [in] base
 *param [out] resultPath 交路数据提取结果的存放路径
 *@return 0: 已经有存放交路数据提取结果的目录，说明已经提取过，则返回0; 1:没有提
 *  取过
 *@discrimination
 *  生成时刻表xml文件的md5码;
 *  生成交路数据存放路径
 *  sbin/route_data/20190329/26012000_6f2997ad46dfabb1d9ab35be60369c6d,
 *  并通过参数resultPath向外输出;
 */
int is_extracted(char *base,char *resultPath);

-------------------------------------------------------------------------------
通信板数据结构
    

全局变量
    CONVERT_MILE_NODE *cm_node 连续公里标

-------------------------------------------------------------------------------
通信板状态机
    【MAINTENANCE_STATUS】//维护状态
        1. 给通信备板发送进入维护状态热备事件[BACKUP_MAINTENANCE_STATUS_EVENT];
        2. 等待接受表决板发送退出维护事件[EXIT_MAINTENANCE_EVENT],接收成功，进入
        【PATTERN_SELECT_STATUS】;//模式选择状态
        3. 接收主备控制盒发送的更新程序事件[UPDATE_APP_EVENT],同步时间，更新程序
    【SYSTEM_STANDBY_STATUS】//系统待机状态
        get_lkj_power()后，调用receive_base_data(),向安全平台发送基础数据请求；
        
-------------------------------------------------------------------------------
6.22
取route_extract目录里的交路提取，单独构建工程，用来调试交路提取功能：
为了单独构建工程编译通过，除了route_extract目录里的代码，还需对route_extract目录
里的代码做一些修改，现记录如下：
    1.在reve_extract.c中，增加get_trip_num(),set_trip_num()函数，已解决函数没有
        定义的编译问题
    2.

-------------------------------------------------------------------------------
分相区文件整理记录：
    1.以前通信板上的dead_zone配置文件中2601200线路第一个分相区的越过距离(分相区
      的预断公里标到上一信号机的距离称为越过距离)824m有错误，应该为794m
    2.通信板上的dead_zone文件中的越过距离为磁枕文件中(预断位置的公里标 - 分相区
      上一信号机的公里标)从新路数据的递增还是递减看哪个时预断位置
    3.通信板上的RawData文件夹中的dead_zone文件(分相区配置文件)
        旧的数据格式为：
        <交路号> <上下行> <分相区前信号机> <预断位越过距离> <cz3 - cz1 + 50>
        <分相区最低限速>
        新的数据格式为：
        <交路号> <上下行> <分相区前信号机> <强断位越过距离> <cz3 - cz2>
        <预断位越过距离> <分相区最低限速>
    4.通信板上的route_data文件夹中的dead_zone文件(交路提取后生成)
        新的数据格式为：
        <上下行> <预断位连续公里标> <cz3 - cz1 + 50> <分相区最低限速> 
        旧的数据格式为：
        <上下行> <强断位连续公里标> <cz3 - cz2 + 100> <预断位连续公里标>
        <分相区最低限速>
    5.芝阳-合阳北车站间的磁枕信息可查看<绥德供电段磁枕台帐201609.27.xls>, 其他的
      磁枕信息可查看<西安供电段磁枕台账20160830.xls>
    6.优化计算工程代码中分相区结构体重新定义如下(./OPTIMIZE/opt_definition.h)：
        struct deadzone {
            int up_down;
            int force_off_post;//location of force to off power
            int cz2cz3_len_add_100m;//
            int read_off_post;//location of read to off power
            float access_min_v;//min velocity of accessing dead_zone
        };
        
-------------------------------------------------------------------------------
route_ext() -> basedate_file_extr() -> basedata_parse()

static ROUTE_EXTR_RESULT basedata_parse(
	struct rt_extrRoadStationTime *train_sch,
	FILE *bd_file, 
	ROUTE_LIST *r_list, 
	STATION_ADDR_T *ss_addr, 
	STATION_ADDR_T *es_addr, 
	uint16_t limit_level);

参数传入：	
*train_sch, 列车时刻表的提取结果，在route_ext()中初始化
*bd_file, basedata.dat的文件流
*r_list, 在route_ext()中申请内存，调用basedate_file_extr()时传入，调用完后再
    route_ext()中free掉

-------------------------------------------------------------------------------
交路提取review记录：
1.limit_merge() -> limit_code_table_extr()；
    limit_code_table_extr()在route_extract.c中定义，定义类型为指针数组
2.limit_merge()函数中，if(cnt_node->value.tele->t_num == 1186)是啥意思；
3.limit_adjust()函数定义前的宏定义：#define AIRWALKDIS 384；
4.limit_adjust()函数里的宏定义：#define spli_dist 2500；
5.limit_merge()函数的参数reve_info在该函数中并没有用到，当时为什么要定义这个参
    数，后续有无作用，没有的话可以将其删除；

-------------------------------------------------------------------------------
通信板程序现状:
    1. 始发站为中间车站时，LKJ要先按开车键后，才会显示优化曲线；
    2. 下行支线3转移测试：后按开车键，支线转移后相应的优化没有出来，不知和后按开
        车键有无关系

-------------------------------------------------------------------------------
函数功能记录
    get_transport_type(): 获取本补客货类型
    set_transport_type(): 获取本补客货类型
    route_extr()：线路数据提取，在main()中调用，在主线程执行，没有开新的线程。在
        揭示信息核对状态JIESHI_INFO_CHECK_STATUS时执行，分上下行调用两次。调用前
        先判断上下行，再判断LKJ的起始车站号是否属于上下行线路，如果属于才会调用
        route_extr()，如果不属于，则置状态机为SYSTEM_ERROR_STATUS
    station_in_num26_route_up(): 判断LKJ输入的起始车站号是否是上行线路的车站
    station_in_num26_route_down(): 判断LKJ输入的起始车站号是否是下行线路的车站
    is_extracted():判断是否已经提取过交路数据
    receive_base_data(): 比较本地基础数据版本和状态信息里的版本，决定是否发送基
        础数据请求命令

/*
 *@brief 判断是否已经提取过交路数据
 *@param [in] base
 *param [out] resultPath 交路数据提取结果的存放路径
 *@return 0: 已经有存放交路数据提取结果的目录，说明已经提取过，则返回0; 1:没有提
 *  取过
 *@discrimination
 *  生成时刻表xml文件的md5码;
 *  生成交路数据存放路径
 *  sbin/route_data/20190329/26012000_6f2997ad46dfabb1d9ab35be60369c6d,
 *  并通过参数resultPath向外输出;
 */
int is_extracted(char *base,char *resultPath);

-------------------------------------------------------------------------------
通信板数据结构
    
全局变量
    CONVERT_MILE_NODE *cm_node 连续公里标

-------------------------------------------------------------------------------
通信板状态机
    【MAINTENANCE_STATUS】//维护状态
        1. 给通信备板发送进入维护状态热备事件[BACKUP_MAINTENANCE_STATUS_EVENT];
        2. 等待接受表决板发送退出维护事件[EXIT_MAINTENANCE_EVENT],接收成功，进入
        【PATTERN_SELECT_STATUS】;//模式选择状态
        3. 接收主备控制盒发送的更新程序事件[UPDATE_APP_EVENT],同步时间，更新程序

        
-------------------------------------------------------------------------------
6.22
取route_extract目录里的交路提取，单独构建工程，用来调试交路提取功能：
为了单独构建工程编译通过，除了route_extract目录里的代码，还需对route_extract目录
里的代码做一些修改，现记录如下：
    1.在reve_extract.c中，增加get_trip_num(),set_trip_num()函数，已解决函数没有
        定义的编译问题
    2.

-------------------------------------------------------------------------------
通信板程序更改记录：
    1.新增记录状态信息和基础数据的接收过程：在debug目录里新增了file_printf.c和
      file_printf.h, 在main()中初始化时调用了file_printf_init(),在接收状态信息和
      基础数据的地方调用了file_printf(),用来记录状态信息和基础数据的接收过程，保
      存在/sbin/status_basedate_records
    2.去掉接收基础数据的功能，临时更改：
      将static int bd_version_local = 0;//临时改为初始化为0;
      get_base_data_version_consistency();//该函数直接返回0，表示本地基础数据和
        状态信息里的版本一致；
      将excan_init1()里对init_local_base_data_version()的调用注销掉;
    
-------------------------------------------------------------------------------
通信板维护：
    1.log日志记录在/var/to_log/里，按日期取最新的
    2.基础数据接收过程的记录信息保存于文件/sbin/status_basedate_records
    3.安全平台数据板数据更新：插上数据转储器后4A灯闪烁，数据正字啊更新；4A停止闪
      烁后，数据更新完毕，拔下转储器；重启安全平台，3A常亮(安全平台和LKJ数据一
      致)或闪烁(安全平台和LKJ数据不一致)，说明数据更新成功
    4.通信板B1灯为心跳指示灯，A2为实时信息指示灯
    5.通信板1.34解析新的基础数据，存储新的交路提取结果前，会把以前的交路提取结果
      的删除，1.35不会覆盖以前的交路提取结果
    6.SVN上TO_COMM_4.0的版本V8669对分相区配置文件及相应的读程序做了改动，而
      TO_CORE_4.0的分相区配置文件没有改动，这两个版本联调时会导致优化失败，要
      注意这一问题
    7.SVN上TO_VOTE_4.0的test.sh脚本中有对开机启动脚本/etc/init.d/rcS的上传操作，
      而TO_VOTE_4.0中又没有rcS文件，执行./test.sh会到删除表决板的开机启动脚本，
      导致表决板启动失败，今天就踩雷了，以后要小心这颗雷。
-------------------------------------------------------------------------------
