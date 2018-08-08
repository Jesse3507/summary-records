-------------------------------------------------------------------------------
优化计算代码工程调试：
    调试时用目录optimeze_3.0_branch3 - copy里的代码工程，调试成功后
    Beyond_compare到目录optimeze_3.0_branch3

-------------------------------------------------------------------------------
函数功能记录
	DoCaculateByDistance() 牵引计算
	DoCaculateByTime() 牵引计算
	post_process() 将结构体OPTSCH转换成OPTSCHCURVE, 在优化曲线输出前之前调用
    pre_process()将结构体OPTSCHCURVE转换成OPTSCH, 在优化曲线计算之前调用
	reverse_tmp_optimize() 反求曲线主入口
	dead_zone_const_gear_reverse() 求进入分相区前的最慢行驶与最快行驶优化曲线
    const_gr_til_***() 惰行正求
    air_break_process_imp 空气制动正求
    air_break_process_rvrs 空气制动反求
    tmp_rvrs_cal_til_post() 恒档反求，函数实现里挡位gear为恒值
    back_set_tmp_optSch() 反向存储
    merge_two_curve() 拼接曲线
    extrDeadzoneInfo() 提取分相区信息
    tmp_limit_start_strgy_from_the_first_station() 降级起车(before)
	thro_expert_optimize() 贯通试验(before)
	tmp_limit_start_strgy() 侧线起车(before)
	tmp_limit_stop_strgy() 侧线停车(before)

-------------------------------------------------------------------------------
代码结构
    ./OPTIMIZE
        datacontrol/
            extractData.c
                

-------------------------------------------------------------------------------
函数内部实现
    thro_expert_strgy()贯通实验策略函数实现：
        确定贯通实验起始点所在坡段的下标rcIndex;
        确定贯通实验起始点所在原始优化曲线的下标start_opt_index;
        给贯通曲线点元素分配空间；
        给贯通曲线第一个点赋值；
        若挡位gear > 0, 调用dec_gr_til_posGp_or_post()降到0档，若到所在第一个坡
        段
            结束未降到0档，则继续在下一个坡段调用dec_gr_til_posGp_or_post()，直
            到降到0档；
        若挡位gear < 0, 调用dec_gr_til_negGp_or_post()降到-120档，若到所在第一个
        坡
            段结束未降到-120档，则继续在下一个坡段调用dec_gr_til_negGp_or_post()
            直到降到-120档；
        维持0档跑5s;
        调用空气制动air_break_process_imp()；
        确定调用完air_break_process_imp()后输出的曲线结束点所对应的目标曲线的下
        标；
        调用dwn_to_up_fwd_cal_til_post()追原始曲线，若遇到分相区，则调用
        pass_split_phase_strgy().找到交点后返回
    
    air_break_process_imp()空气制动
        根据坡度算减压值；
        计算空走时间；

    tmp_limit_start_strgy()原来代码中的起车曲线函数
        计算起车点对应的分段坡度的下标startRcIndex；
        计算起车点对应的原始优化的下标startOptIndex；
        给起车曲线第一个点赋值；
        调用const_gr_til_time_or_post()恒档跑5s;
        判断恒档5s后的速度，若小于零，则取最后一个速度大于零的点；
        若flag == -1,则更改下次调用const_gr_til_time_or_post()的入参time;
        判断恒档5s后是否跑过10m, 若没有，则更改档位，循环调用const_gr_til_post()
        直至跑过10m;
        追原始曲线；
        
        #疑点
        1.maxV为什么赋了两次值，后面也没有用到，是否可注销掉
        2.变量mc也没有用到

-------------------------------------------------------------------------------
数据结构记录
    优化计算用到的数据结构定义在opt_definition.h文件中
    OPTSCHCURVE结构体 用来描述优化曲线
    新定义了结构体用来表示过分相测算的输出曲线
        struct deadzone_base {
            OPTSCHCURVE **access_deadzone_min;
            OPTSCHCURVE **access_deadzone_max;
            DEADZONE *deadzone_struct;
        };
        typedef struct deadzone_base DEADZONE_BASE;

-------------------------------------------------------------------------------
全局变量
    在extractData.c文件中，定义了一些全局变量；
    opt_global_var.h中，定义了一些全局变量；

    MAX_ROADCATEGORY, 坡段实际长度，在函数computeRoadCategory()中对其赋值

-------------------------------------------------------------------------------
项目相关
    1.通信板上的分相区配置文件deadzone的数据含义为依次为：交路号；上下行；
      分相区前一个信号机；越过距离(分相区起点到前一个信号机的距离)；分相区长度；
      最小速度；
    2.在优化程序RawData文件夹中的optimize_strategy.xml，对坡度类型做了定义
	  roadCategory结构体中flag的含义:
	  3分相区
	  2陡上坡
	  0缓坡
	  -2陡下坡

-------------------------------------------------------------------------------
#疑点
    优化计算工程里的分相区文件内容与交路提取里的分相区文件内容不一致(7.11)

-------------------------------------------------------------------------------
调试记录：
    贯通试验：
    1.为了实现贯通试验函数tmp_limit_opt_thro_expert_strgy()中对偏离追踪
      tmp_limit_opt_forward_strgy()的调用，先将optimeze_3.0_branch3 - copy中的
      tmp_limit_opt_forward_strgy_imp.c从branch1更新

    过分相测算：
    1.过分相测算中调用dead_zone_const_gear_reverse()做电制反求后，速度越来会越
      小，正常情况应越来越大，将dead_zone_const_gear_reverse()里的
      'v = v + delta_v'改成'v = v - delta_v'后，电制反求后速度越来越大

-------------------------------------------------------------------------------
问题记录：
    停车策略里调用空气制动的传参有问题

-------------------------------------------------------------------------------
优化计算代码工程调试：
    调试时用目录optimeze_3.0_branch3 - copy里的代码工程，调试成功后
    Beyond_compare到目录optimeze_3.0_branch3

-------------------------------------------------------------------------------
在优化程序RawData文件夹中的optimize_strategy.xml，对坡度类型做了定义
	roadCategory结构体中flag的含义:
	3分相区
	2陡上坡
	0缓坡
	-2陡下坡

-------------------------------------------------------------------------------
侧线正线起停车、降级起车，贯通试验
	降级起车tmp_limit_start_strgy_from_the_first_station()
	贯通试验thro_expert_optimize()
	侧线起车tmp_limit_start_strgy()
	侧线停车tmp_limit_stop_strgy()

-------------------------------------------------------------------------------
起停车，过分相测算，贯通试验策略里用到的子操作
    限速起车：
-------------------------------------------------------------------------------
侧线正线起停车、降级起车，贯通试验
	降级起车tmp_limit_start_strgy_from_the_first_station()
	贯通试验thro_expert_optimize()
	侧线起车tmp_limit_start_strgy()
    侧线停车tmp_limit_stop_strgy()

-------------------------------------------------------------------------------
函数功能记录
	DoCaculateByDistance() 牵引计算
	DoCaculateByTime() 牵引计算
	post_process() 将结构体OPTSCH转换成OPTSCHCURVE, 在优化曲线输出前之前调用
    pre_process()将结构体OPTSCHCURVE转换成OPTSCH, 在优化曲线计算之前调用
	reverse_tmp_optimize() 反求曲线主入口
	dead_zone_const_gear_reverse() 求进入分相区前的最慢行驶与最快行驶优化曲线
    const_gr_til_***() 惰行正求
    air_break_process_imp 空气制动正求
    air_break_process_rvrs 空气制动反求
    tmp_rvrs_cal_til_post() 恒档反求，函数实现里挡位gear为恒值
    back_set_tmp_optSch() 反向存储
    merge_two_curve() 拼接曲线
    extrDeadzoneInfo() 提取分相区信息

-------------------------------------------------------------------------------
函数内部实现
    thro_expert_strgy()贯通实验策略函数实现：
        确定贯通实验起始点所在坡段的下标rcIndex;
        确定贯通实验起始点所在原始优化曲线的下标start_opt_index;
        给贯通曲线点元素分配空间；
        给贯通曲线第一个点赋值；
        若挡位gear > 0, 调用dec_gr_til_posGp_or_post()降到0档，若到所在第一个坡
            段结束未降到0档，则继续在下一个坡段调用dec_gr_til_posGp_or_post()，
            直到降到0档；
        若挡位gear < 0, 调用dec_gr_til_negGp_or_post()降到-120档，若到所在第一个
        坡段结束未降到-120档，则继续在下一个坡段调用dec_gr_til_negGp_or_post()，
        直到降到-120档；
        维持0档跑5s;
        调用空气制动air_break_process_imp()；
        确定调用完air_break_process_imp()后输出的曲线结束点所对应的目标曲线的下
        标；
        调用dwn_to_up_fwd_cal_til_post()追原始曲线，若遇到分相区，则调用
        pass_split_phase_strgy().找到交点后返回
    
    air_break_process_imp()空气制动
        根据坡度算减压值；
        计算空走时间；

-------------------------------------------------------------------------------
数据结构记录
    优化计算用到的数据结构定义在opt_definition.h文件中
    OPTSCHCURVE结构体 用来描述优化曲线
    新定义了结构体用来表示过分相测算的输出曲线
        struct deadzone_base {
        OPTSCHCURVE **access_deadzone_min;
        OPTSCHCURVE **access_deadzone_max;
        DEADZONE *deadzone;
        };
        typedef struct deadzone_base DEADZONE_BASE;

-------------------------------------------------------------------------------
全局变量
    在extractData.c文件中，定义了一些全局变量；

-------------------------------------------------------------------------------
tmp_limit_start_strgy()原来代码中的起车曲线函数
计算起车点对应的分段坡度的下标startRcIndex；
计算起车点对应的原始优化的下标startOptIndex；
给起车曲线第一个点赋值；
调用const_gr_til_time_or_post()恒档跑5s;
判断恒档5s后的速度，若小于零，则取最后一个速度大于零的点；
若flag == -1,则更改下次调用const_gr_til_time_or_post()的入参time;
判断恒档5s后是否跑过10m, 若没有，则更改档位，循环调用const_gr_til_post(),直至跑过10m;
追原始曲线；
疑点
1.maxV为什么赋了两次值，后面也没有用到，是否可注销掉
2.变量mc也没有用到

dead_zone_const_gear_reverse()计算过分相前的最低限速曲线和最高限速曲线

