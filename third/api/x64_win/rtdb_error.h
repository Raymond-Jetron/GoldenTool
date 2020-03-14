#ifndef __GOLDEN_ERROR_H__
#define __GOLDEN_ERROR_H__

#define GoE_UNKNOWN_ERROR                                   0xFFFFFFFF  // 未知错误
#define GoE_OK                                              0x00000000  // 操作成功完成
#define GoE_WINDOWS_ERROR                                   0xFFF00000  // Windows操作系统错误的起始值
#define GoE_WINDOWS_ERROR_MAX                               0xFFFF0000  // Windows操作系统错误的结束值

#define GOLDEN_SOCKET_ADAPTER_FALSE(GOLDEN_ERROR) (GOLDEN_ERROR > 0xFFF02710 && GOLDEN_ERROR <= 0xFFF02AFC)
#define GOLDEN_WINDOWS_ERROR_TO_GOLDEN(WINDOWS_ERROR) (GoE_WINDOWS_ERROR + WINDOWS_ERROR)

#define GoE_INVALID_OPENMODE                                0xFFFF0001  // 无效的文件打开方式
#define GoE_OPENFILE_FAILED                                 0xFFFF0002  // 打开文件失败
#define GoE_MOVETOEND_FAILED                                0xFFFF0003  // 移动文件指针到文件尾失败
#define GoE_DIFFER_READBYTES                                0xFFFF0004  // 读取内容长度与要求不符
#define GoE_GETFILEPOS_FAILED                               0xFFFF0005  // 获取当前文件指针失败
#define GoE_FLUSHFILE_FAILED                                0xFFFF0006  // 刷新文件缓冲区失败
#define GoE_SETSIZE_FAILED                                  0xFFFF0007  // 设置文件大小失败
#define GoE_FILE_NOT_CLOSED                                 0xFFFF0008  // 试图用未关闭的文件对象创建或打开文件
#define GoE_FILE_UNKNOWN                                    0xFFFF0009  // 创建或打开文件时必须指定文件名
#define GoE_INVALID_HEADER                                  0xFFFF000A  // 数据文件头信息错误
#define GoE_DISABLED_FILE                                   0xFFFF000B  // 数据文件无效，试图访问无效数据文件
#define GoE_FILE_NOT_OPENED                                 0xFFFF000C  // 试图访问尚未打开的数据文件
#define GoE_POINT_NOT_FOUND                                 0xFFFF000D  // 要求访问的标签点不存在或无效
#define GoE_READYBLOCK_NOT_FOUND                            0xFFFF000E  // 数据文件中找不到从指定数据块以后的可用的空块
#define GoE_FILE_IS_INCULT                                  0xFFFF000F  // 文件未被使用过
#define GoE_FILE_IS_FULL                                    0xFFFF0010  // 数据文件已满
#define GoE_FILEEX_IS_FULL                                  0xFFFF0011  // 数据文件扩展区已满，无法继续装载数据
#define GoE_INVALID_DATA_TYPE                               0xFFFF0012  // 无效的数据类型
#define GoE_DATABLOCK_NOT_FOUND                             0xFFFF0013  // 找不到符合时间条件的数据块
#define GoE_DATA_BETWEEN_BLOCK                              0xFFFF0014  // 数据时间位于找到的块和下一个数据块之间
#define GoE_CANT_MODIFY_EXIST_VALUE                         0xFFFF0015  // 不允许修改已存在的数据
#define GoE_WRONGDATA_IN_BLOCK                              0xFFFF0016  // 块中有错误数据导致数据块头信息不符
#define GoE_DATATIME_NOT_IN                                 0xFFFF0017  // 数据文件中没有该标签点指定时间的数据
#define GoE_NULL_ARCHIVE_PATH                               0xFFFF0018  // 操作的数据文件路径为空
#define GoE_REG_ARCHIVE_PATH                                0xFFFF0019  // 数据文件已被注册
#define GoE_UNREG_ARCHIVE_PATH                              0xFFFF001A  // 未注册的数据文件
#define GoE_FILE_INEXISTENCE                                0xFFFF001B  // 指定的文件不存在
#define GoE_DATA_TYPE_NOT_MATCH                             0xFFFF001C  // 数据类型不匹配
#define GoE_FILE_IS_READONLY                                0xFFFF001D  // 不允许修改只读数据文件中的数据
#define GoE_TOMANY_ARCHIVE_FILE                             0xFFFF001E  // 过多的数据文件
#define GoE_NO_POINTS_LIST                                  0xFFFF001F  // 缺少标签点列表
#define GoE_NO_ACTIVED_ARCHIVE                              0xFFFF0020  // 缺少活动文档
#define GoE_NO_ARCHIVE_FILE                                 0xFFFF0021  // 缺少数据文档
#define GoE_NEED_ACTIVED_ARCHIVE                            0xFFFF0022  // 只能在活动文档上执行该操作
#define GoE_INVALID_TIMESTAMP                               0xFFFF0023  // 无效的时间戳
#define GoE_NEED_MORE_WRITABLE                              0xFFFF0024  // 非只读文档个数太少
#define GoE_NO_ARCHIVE_FOR_PUT                              0xFFFF0025  // 找不到合适的追加历史数据的文档
#define GoE_INVALID_VALUE_MODE                              0xFFFF0026  // 无效的取值模式
#define GoE_DATA_NOT_FOUND                                  0xFFFF0027  // 找不到需要的数据
#define GoE_INVALID_PARAMETER                               0xFFFF0028  // 无效的参数
#define GoE_REDUPLICATE_TAG                                 0xFFFF0029  // 重复的标签点名
#define GoE_REDUPLICATE_TABNAME                             0xFFFF002A  // 重复的表名称
#define GoE_REDUPLICATE_TABID                               0xFFFF002B  // 重复的表ID
#define GoE_TABLE_NOT_FOUND                                 0xFFFF002C  // 指定的表不存在
#define GoE_UNSUPPORTED_CLASSOF                             0xFFFF002D  // 不支持的标签点类别
#define GoE_WRONG_OR_DUPLIC_TAG                             0xFFFF002E  // 错误的或重复的标签点名
#define GoE_REDUPLICATE_PT                                  0xFFFF002F  // 重复的标签点标识
#define GoE_POINT_LICENSE_FULL                              0xFFFF0030  // 标签点数超出了许可证规定的最大数目
#define GoE_TABLE_LICENSE_FULL                              0xFFFF0031  // 标签点表个数超出了许可证规定的最大数目
#define GoE_WRONG_OR_DUPLIC_TABNAME                         0xFFFF0032  // 错误的或重复的表名称
#define GoE_INVALID_FILE_FORMAT                             0xFFFF0033  // 无效的数据文件格式
#define GoE_WRONG_TABNAME                                   0xFFFF0034  // 错误的表名称
#define GoE_WRONG_TAG                                       0xFFFF0035  // 错误的标签点名
#define GoE_NOT_IN_SCOPE                                    0xFFFF0036  // 数值超出了应属的范围
#define GoE_CANT_LOAD_BASE                                  0xFFFF0037  // 不能同标签点信息服务取得联系
#define GoE_CANT_LOAD_SNAPSHOT                              0xFFFF0038  // 不能同快照数据服务取得联系
#define GoE_CANT_LOAD_HISTORY                               0xFFFF0039  // 不能同历史数据服务取得联系
#define GoE_CANT_LOAD_EQUATION                              0xFFFF003A  // 不能同实施方程式服务取得联系
#define GoE_ARRAY_SIZE_NOT_MATCH                            0xFFFF003B  // 数组尺寸不匹配
#define GoE_INVALID_HOST_ADDRESS                            0xFFFF003C  // 无效的主机地址
#define GoE_CONNECT_FALSE                                   0xFFFF003D  // 连接已断开
#define GoE_TOOMANY_BYTES_RECVED                            0xFFFF003E  // 接收到的数据长度超出了指定字节长度
#define GoE_REQID_RESPID_NOT_MATCH                          0xFFFF003F  // 应答与请求的ID不一致
#define GoE_LESS_BYTES_RECVED                               0xFFFF0040  // 接收到的数据长度小于指定字节长度
#define GoE_UNSUPPORTED_CALC_MODE                           0xFFFF0041  // 不支持的计算模式
#define GoE_UNSUPPORTED_DATA_TYPE                           0xFFFF0042  // 不支持的标签点类型
#define GoE_INVALID_EXPRESSION                              0xFFFF0043  // 无效的表达式
#define GoE_INCOND_DATA_NOT_FOUND                           0xFFFF0044  // 找不到符合条件的数据
#define GoE_VALID_DATA_NOT_FOUND                            0xFFFF0045  // 找不到需要的有效数据
#define GoE_VALUE_OR_STATE_IS_NAN                           0xFFFF0046  // 数据或状态不正常，为NAN
#define GoE_CREATE_MUTEX_FAILED                             0xFFFF0047  // 创建互斥对象失败
#define GoE_TLSALLOCFAIL                                    0xFFFF0048  // 处理TLS时调用系统函数LocalAlloc()失败，可能因为内存不足导致
#define GoE_TO_MANY_POINTS                                  0xFFFF0049  // 正在调用的API函数不支持过多的标签点数量，请参考函数声明和开发手册
#define GoE_LIC_INFO_ERROR                                  0xFFFF004A  // 获取授权许可协议信息时发生错误
#define GoE_ARCHIVE_BUFFER_FULL                             0xFFFF004B  // 标签点的历史数据补写缓冲区已满，请稍后再补
#define GoE_USER_NOT_EXIST                                  0xFFFF004C  // 用户不存在
#define GoE_USER_IS_LOCKED                                  0xFFFF004D  // 帐户被锁定,需要管理员为您解锁
#define GoE_WRONG_PASSWORD                                  0xFFFF004E  // 错误的口令
#define GoE_ACCESS_IS_DENIED                                0xFFFF004F  // 访问被拒绝，请确定是否具有足够的权限
#define GoE_HAVE_NOT_LOGIN                                  0xFFFF0050  // 您尚未登录，请先登录
#define GoE_USER_IS_DELETED                                 0xFFFF0051  // 帐户已被删除
#define GoE_USER_ALREADY_EXIST                              0xFFFF0052  // 帐户已存在
#define GoE_WRONG_CREATE_TABNAME                            0xFFFF0053  // 创建删除表失败
#define GoE_WRONG_FIELD_VALUE                               0xFFFF0054  // 标签点属性值有错误
#define GoE_INVALID_TAG_ID                                  0xFFFF0055  // 无效的标签点ID
#define GoE_CHECK_NAMED_TYPE_NAME_ERROR                     0xFFFF0056  // 无效的自定义类型名称或字段名称
#define GoE_CANT_LOAD_DISPATCH                              0xFFFF0057  // 不能同转发服务器取得联系
#define GoE_CONNECT_TIME_OUT                                0xFFFF0058  // 连接已超时，需要重新登录
#define GoE_WRONG_LOGIN_4                                   0xFFFF0059  // 账户信息验证失败，还有4次尝试机会
#define GoE_WRONG_LOGIN_3                                   0xFFFF005A  // 账户信息验证失败，还有3次尝试机会
#define GoE_WRONG_LOGIN_2                                   0xFFFF005B  // 账户信息验证失败，还有2次尝试机会
#define GoE_WRONG_LOGIN_1                                   0xFFFF005C  // 账户信息验证失败，还有1次尝试机会
#define GoE_WRONG_DESC										0xFFFF005D  //!< 错误的表描述
#define GoE_WRONG_UNIT										0xFFFF005E  //!< 错误的工程单位
#define GoE_WRONG_CHANGER									0xFFFF005F  //!< 错误的最后一次被修改的用户名 
#define GoE_WRONG_CREATOR									0xFFFF0060  //!< 错误的标签点创建者用户名
#define GoE_WRONG_FULL										0xFFFF0061  //!< 错误的标签点全名
#define GoE_WRONG_SOURCE									0xFFFF0062  //!< 错误的数据源
#define GoE_WRONG_INSTRUMENT								0xFFFF0063  //!< 错误的设备标签
#define GoE_WRONG_USER										0xFFFF0064  //!< 错误的创建者
#define GoE_WRONG_EQUATION									0xFFFF0065  //!< 错误的实时方程式
#define GoE_WRONG_TYPE_NAME									0xFFFF0066  //!< 错误的自定义类型名称
#define GoE_WRONG_ENCODE									0xFFFF0067  //!< 编码转换时出错
#define GoE_WRONG_OTHERMASK									0xFFFF0068  //!< 错误的搜索类型转换mask值
#define GoE_WRONG_TYPE										0xFFFF0069  //!< 错误的搜索类型
#define GoE_POINT_HARDWARE_LIMITED                          0xFFFF0070  // 由于硬件资源限制，创建或恢复标签点失败


// v2.1 increased errors
#define GoE_NO_FREE_TABLE_ID                                0xFFFF1001  // 没有空闲的表ID可用
#define GoE_NO_FREE_TAG_POSITION                            0xFFFF1002  // 没有空闲的标签点位址可用
#define GoE_NO_FREE_SCAN_TAG_POSITION                       0xFFFF1003  // 没有空闲的采集标签点位址可用
#define GoE_NO_FREE_CALC_TAG_POSITION                       0xFFFF1004  // 没有空闲的计算标签点位址可用
#define GoE_INVALID_IPC_POSITION                            0xFFFF1005  // 无效的位址被用于进程间内存共享
#define GoE_WRONG_IPC_POSITION                              0xFFFF1006  // 错误的位址被用于进程间内存共享
#define GoE_IPC_ACCESS_EXCEPTION                            0xFFFF1007  // 共享内存访问异常
#define GoE_ARV_PAGE_NOT_READY                              0xFFFF1008  // 没有空闲的历史数据缓存页
#define GoE_ARVEX_PAGE_NOT_READY                            0xFFFF1009  // 没有空闲的补历史数据缓存页
#define GoE_INVALID_POSITION_FROM_ID                        0xFFFF100A  // 依据标签点ID获得的位址无效
#define GoE_NO_ACTIVE_PAGE_ALLOCATOR                        0xFFFF100B  // 新的活动存档无法加载页分配器
#define GoE_MAP_IS_NOT_READY                                0xFFFF100C  // 内存映射尚未就绪
#define GoE_FILE_MAP_FAILED                                 0xFFFF100D  // 文件映射到内存失败
#define GoE_TIME_RANGE_NOT_ALLOWED                          0xFFFF100E  // 不允许使用的时间区间
#define GoE_NO_DATA_FOR_SUMMARY                             0xFFFF100F  // 找不到用于统计的源数据
#define GoE_CANT_OPERATE_ON_ACTIVED                         0xFFFF1010  // 不允许操作活动存档文件
#define GoE_SCAN_POINT_LICENSE_FULL                         0xFFFF1011  // 采集标签点数超出了许可证规定的最大数目
#define GoE_CALC_POINT_LICENSE_FULL                         0xFFFF1012  // 计算标签点数超出了许可证规定的最大数目
#define GoE_HISTORIAN_IS_SHUTTINGDOWN                       0xFFFF1013  // 历史数据服务正在停止
#define GoE_SNAPSHOT_IS_SHUTTINGDOWN                        0xFFFF1014  // 实时数据服务正在停止
#define GoE_EQUATION_IS_SHUTTINGDOWN                        0xFFFF1015  // 实时方程式服务正在停止
#define GoE_BASE_IS_SHUTTINGDOWN                            0xFFFF1016  // 标签点信息服务正在停止
#define GoE_SERVER_IS_SHUTTINGDOWN                          0xFFFF1017  // 网络通信服务正在停止
#define GoE_OUT_OF_MEMORY                                   0xFFFF1018  // 内存不足
#define GoE_INVALID_PAGE                                    0xFFFF1019  // 无效的数据页，有可能是未加载
#define GoE_PAGE_IS_EMPTY                                   0xFFFF101A  // 遇到空的数据页
#define GoE_STR_OR_BLOB_TOO_LONG                            0xFFFF101B  // 字符串或BLOB数据长度超出限制
#define GoE_CREATED_OR_OVERDUE                              0xFFFF101C  // 尚未产生任何快照或快照已过期
#define GoE_ARCHIVE_INFO_NOT_MATCHING                       0xFFFF101D  // 历史存档文件头部信息与实际不符
#define GoE_TIME_RANGE_OVERLAPPING                          0xFFFF101E  // 指定的时间范围与已有存档文件重叠
#define GoE_CANNOT_SHIFT_TO_ACTIVED                         0xFFFF101F  // 找不到合适的存档文件用于切换成活动存档
#define GoE_INDEX_NOT_READY                                 0xFFFF1020  // 历史存档文件对应的索引尚未就绪
#define GoE_INDEX_NODE_NOT_MATCH                            0xFFFF1021  // 索引节点与指向的内容不符
#define GoE_CAN_NOT_CREATE_INDEX                            0xFFFF1022  // 无法创建索引节点
#define GoE_CAN_NOT_REMOVE_INDEX                            0xFFFF1023  // 无法删除索引节点
#define GoE_INVALID_FILTER_EXPRESS                          0xFFFF1024  // 无效的过滤器表达式
#define GoE_MORE_VAR_IN_FILTER_EXP                          0xFFFF1025  // 过滤器表达式中的包含了过多的变量
#define GoE_INVALID_ARV_PAGE_ALLOCATE                       0xFFFF1026  // 刚分配的历史数据缓存页ID与标签点事件对象ID不匹配
#define GoE_INVALID_ARVEX_PAGE_ALLOCATE                     0xFFFF1027  // 刚分配的补历史数据缓存页ID与标签点事件对象ID不匹配
#define GoE_BIG_JOB_IS_NOT_DONE                             0xFFFF1028  // 正在执行重要的任务，请稍后再试
#define GoE_DATABASE_NEED_RESTART                           0xFFFF1029  // 数据库需要重新启动以便应用新的参数
#define GoE_INVALID_TIME_FORMAT                             0xFFFF102A  // 无效的时间格式字符串
#define GoE_DATA_PLAYBACK_DONE                              0xFFFF102B  // 历史数据回放过程已结束
#define GoE_BAD_EQUATION                                    0xFFFF102C  // 错误的方程式
#define GoE_NOT_ENOUGH_SAPCE                                0xFFFF102D  // 剩余磁盘空间不足
#define GoE_ACTIVED_ARCHIVE_EXIST                           0xFFFF102E  // 已存在活动存档
#define GoE_ARCHIVE_HAVE_EX_FILES                           0xFFFF102F  // 指定的存档文件具有附属文件
#define GoE_ARCHIVE_IS_NOT_LATEST                           0xFFFF1030  // 指定的存档文件不是最晚的
#define GoE_DB_SYSTEM_NOT_RUNNING                           0xFFFF1031  // 数据库管理系统尚未完全启动
#define GoE_ARCHIVE_IS_ALTERED                              0xFFFF1032  // 存档文件内容发生变更
#define GoE_ARCHIVE_IS_TOO_SMALL                            0xFFFF1033  // 不允许创建太小的存档文件和附属文件
#define GoE_INVALID_INDEX_NODE                              0xFFFF1034  // 遇到无效的索引节点
#define GoE_MODIFY_SNAPSHOT_NOT_ALLOWED                     0xFFFF1035  // 不允许删除或修改快照事件
#define GoE_SEARCH_INTERRUPTED                              0xFFFF1036  // 因目标正被创建、删除或恢复，搜索被迫中断，请稍后再试
#define GoE_RECYCLE_SHUTDOWN                                0xFFFF1037  // 回收站已失效，相关操作无法完成
#define GoE_NEED_TO_REINDEX                                 0xFFFF1038  // 索引文件缺失，或部分索引节点被损坏，需要重建索引
#define GoE_INVALID_QUALITY                                 0xFFFF1039  // 无效的质量码
#define GoE_EQUATION_NOT_READY                              0xFFFF103A  // 实时方程式服务正在解析，请稍后再试
#define GoE_ARCHIVES_LICENSE_FULL                           0xFFFF103B  // 存档文件数已达到许可证规定的最大数目
#define GoE_RECYCLED_LICENSE_FULL                           0xFFFF103C  // 标签点回收站容量超出了许可证规定的最大数目
#define GoE_STR_BLOB_LICENSE_FULL                           0xFFFF103D  // 字符串或BLOB类型标签点数量超出了许可证规定的最大数目
#define GoE_NOT_SUPPORT_WHEN_DEBUG                          0xFFFF103E  // 此功能被某个调试选项禁用
#define GoE_MAPPING_ALREADY_LOADED                          0xFFFF103F  // 映射已经被加载，不允许重复加载
#define GoE_ARCHIVE_IS_MODIFIED                             0xFFFF1040  // 存档文件被修改，动作被中断
#define GoE_ACTIVE_ARCHIVE_FULL                             0xFFFF1041  // 活动文档已满
#define GoE_SPLIT_NO_DATA                                   0xFFFF1042  // 拆分数据页后所给时间区间内没有数据
#define GoE_INVALID_DIRECTORY                               0xFFFF1043  // 指定的路径不存在或无效
#define GoE_ARCHIVE_LACK_EX_FILES                           0xFFFF1044  // 指定存档文件的部分附属文件缺失
#define GoE_BIG_JOB_IS_CANCELED                             0xFFFF1045  // 后台任务被取消

#define GoE_ARVEX_BLOB_PAGE_NOT_READY                       0xFFFF1046  // 没有空闲的blob补历史数据缓存页
#define GoE_INVALID_ARVEX_BLOB_PAGE_ALLOCATE                0xFFFF1047  // 刚分配的blob补历史数据缓存页ID与标签点事件对象ID不匹配

#define GoE_TIMESTAMP_EQUALTO_SNAPSHOT                      0xFFFF1048  // 写入的时间与快照时间相同
#define GoE_TIMESTAMP_EARLIER_THAN_SNAPSHOT                 0xFFFF1049  // 写入的时间比当前快照时间较早
#define GoE_TIMESTAMP_GREATER_THAN_ALLOW                    0xFFFF104A  // 写入的时间超过了允许的时间
#define GoE_TIMESTAMP_BEGINTIME_GREAGER_THAN_ENDTIME        0xFFFF104B  // 开始时间大于结束时间
#define GoE_TIMESTAMP_BEGINTIME_EQUALTO_ENDTIME             0xFFFF104C  // 开始时间等于结束时间

#define GoE_INVALID_COUNT                                   0xFFFF104D  //无效的count
#define GoE_INVALID_CAPACITY                                0xFFFF104E  //无效的capacity
#define GoE_INVALID_PATH                                    0xFFFF104F  //无效的路径
#define GoE_INVALID_POSITION                                0xFFFF1050  //无效的position
#define GoE_INVALID_ARV_PAGE                                0xFFFF1051  //无效的golden_arv_page<GOLDEN_T>,未加载，或者size小于等于0
#define GoE_INVALID_HISINFO_ITEM_STATE                      0xFFFF1052
#define GoE_INVALID_INTERVAL                                0xFFFF1053
#define GoE_INVALID_LENGTH                                  0xFFFF1054  //无效的字符串长度
#define GoE_INVALID_SERACH_MODE                             0xFFFF1055  //无效的search mode
#define GoE_INVALID_FILE_ID                                 0xFFFF1056  //无效的存档文件ID
#define GoE_INVALID_MILLISECOND                             0xFFFF1057  //无效的毫秒值
#define GoE_INVALID_DEADLINE                                0xFFFF1058  //无效的截止时间
#define GoE_INVALID_JOBNAME                                 0xFFFF1059  //
#define GoE_INVALID_JOBSTATE                                0xFFFF105A  //
#define GoE_INVALID_PROCESS_RATE                            0xFFFF105B  //




#define GoE_INVALID_TABLE_ID                                0xFFFF105C  // 无效的表ID
#define GoE_INVALID_DATA_SOURCE                             0xFFFF105D  // 无效的数据源格式
#define GoE_INVALID_TRIGGER_METHOD                          0xFFFF105E  // 无效的触发方式
#define GoE_INVALID_CALC_TIME_RES                           0xFFFF105F  // 无效的计算结果时间戳参考方式
#define GoE_INVALID_TRIGGER_TIMER                           0xFFFF1060  // 无效的定时触发触发周期,不能小于1秒
#define GoE_INVALID_LIMIT                                   0xFFFF1061  // 工程上限不得低于工程下限
#define GoE_INVALID_COMP_TIME                               0xFFFF1062  // 无效的压缩间隔，最长压缩间隔不得小于最短压缩间隔
#define GoE_INVALID_EXT_TIME                                0xFFFF1063  // 无效的例外间隔，最长例外间隔不得小于最短例外间隔
#define GoE_INVALID_DIGITS                                  0xFFFF1064  // 无效的数值位数，数值位数超出了范围,-20~10
#define GoE_INVALID_FULL_TAG_NAME                           0xFFFF1065  // 标签点全名有误，找不到表名与点名的分隔符“.”
#define GoE_INVALID_TABLE_DESC                              0xFFFF1066  // 表描述信息过有误
#define GoE_INVALID_USER_COUNT                              0xFFFF1067  // 非法的用户个数，小于0
#define GoE_INVALID_BLACKLIST_COUNT                         0xFFFF1068  // 非法的黑名单个数，小于0
#define GoE_INVALID_AUTHORIZATION_COUNT                     0xFFFF1069  // 非法的信任连接个数，小于0
#define GoE_INVALID_BIG_JOB_TYPE                            0xFFFF106A  // 非法的大任务类型
#define GoE_INVALID_SYS_PARAM                               0xFFFF106B  // 无效的系统参数，调用db_set_db_info2时，参数有误
#define GoE_INVALID_FILE_PARAM                              0xFFFF106C  // 无效的文件路径参数，调用db_set_db_info1时，参数有误
#define GoE_INVALID_FILE_SIZE                               0xFFFF106D  // 文件长度有误  < 1 baserecycle.dat、scanrecycle.dat、calcrecycle.dat、snaprecycle.dat
#define GoE_INVALID_TAG_TYPE                                0xFFFF106E  // 标签点类型有误，合法（ golden_bool ~ golden_blob)，但是不属于相应函数的处理范围
#define GoE_INVALID_RECY_STRUCT_POS                         0xFFFF106F  // 回收站对象最后一个结构体位置非法
#define GoE_INVALID_RECYCLE_FILE                            0xFFFF1070  // scanrecycle.dat、baserecycle.dat  snaprecycle.dat文件不存在或失效
#define GoE_INVALID_SUFFIX_NAME                             0xFFFF1071  // 无效的文件后缀名

#define GoE_INSERT_STRING_FALSE                             0xFFFF1072  // 向数据页中插入字符串数据失败
#define GoE_BLOB_PAGE_FULL                                  0xFFFF1073  // blob数据页已满
#define GoE_INVALID_STRING_ITERATOR_POINTER                 0xFFFF1074  // 无效的str/blob迭代器指针
#define GoE_NOT_EQUAL_TAGID                                 0xFFFF1075  // 目标页标签点ID 与 当前ID不一致
#define GoE_PATHS_OF_ARCHIVE_AND_AUTOBACK_ARE_SAME          0xFFFF1076  // 存档文件路径与自动备份路径相同
#define GoE_XML_PARSE_FAIL                                  0xFFFF1077  // xml文件解析失败
#define GoE_XML_ELEMENTS_ABSENT                             0xFFFF1078  // xml清单文件文件内容缺失
#define GoE_XML_MISMATCH_ON_NAME                            0xFFFF1079  // xml清单文件与本产品不匹配
#define GoE_XML_MISMATCH_ON_VERSION                         0xFFFF107A  // xml清单文件版本不匹配
#define GoE_XML_MISMATCH_ON_DATASIZE                        0xFFFF107B  // xml清单文件数据尺寸不匹配
#define GoE_XML_MISMATCH_ON_FILEINFO                        0xFFFF107C  // xml清单文件中数据文件信息不匹配
#define GoE_XML_MISMATCH_ON_WINDOW                          0xFFFF107D  // xml清单文件中所有数据文件的窗口大小必须一致

#define GoE_XML_MISMATCH_ON_TYPECOUNT                       0xFFFF107F  // xml清单文件自定义数据类型的数量不匹配
#define GoE_XML_MISMATCH_ON_FIELDCOUNT                      0xFFFF1080  // xml清单文件自定义数据类型的field不匹配
#define GoE_XML_FIELD_MUST_IN_TYPE                          0xFFFF1081  // xml清单文件中field标签必须嵌套在type标签中
#define GoE_INVALID_NAMED_TYPE_FIELD_COUNT                  0xFFFF1082  // 无效的FIELD数量
#define GoE_REDUPLICATE_FIELD_NAME                          0xFFFF1083  // 重复的FIELD名字
#define GoE_INVALID_NAMED_TYPE_NAME                         0xFFFF1084  // 无效的自定义数据类型的名字
#define GoE_REDUPLICATE_NAMED_TYPE                          0xFFFF1085  // 已经存在的自定义数据类型
#define GoE_NOT_EXIST_NAMED_TYPE                            0xFFFF1086  // 不存在的自定义数据类型
#define GoE_UPDATE_XML_FAILED                               0xFFFF1087  // 更新XML清单文件失败
#define GoE_NAMED_TYPE_USED_WITH_POINT                      0xFFFF1088  // 有些标签点正在使用此自定义数据类型，不允许删除
#define GoE_NAMED_TYPE_UNSUPPORT_CALC_POINT                 0xFFFF1089  // 自定义数据类型不支持计算点
#define GoE_XML_MISMATCH_ON_MAX_ID                          0xFFFF1090  // 自定义数据类型的最大ID与实际的自定义数据类型数量不一致
#define GoE_NAMED_TYPE_LICENSE_FULL                         0xFFFF1091  // 自定义数据类型的数量超出了许可证规定的最大数目
#define GoE_NO_FREE_NAMED_TYPE_ID                           0xFFFF1092  // 没有空闲的自定义数据类型的ID
#define GoE_INVALID_NAMED_TYPE_ID                           0xFFFF1093  // 无效的自定义数据类型ID
#define GoE_INVALID_NAMED_TYPE_FIELD_NAME                   0xFFFF1094  // 无效的自定义数据类型的字段名字
#define GoE_NAMED_TYPE_USED_WITH_RECYCLE_POINT              0xFFFF1095  // 有些回收站中的标签点正在使用此自定义数据类型，不允许删除
#define GoE_NAMED_TYPE_NAME_TOO_LONG                        0xFFFF1096  // 自定义类型的名字超过了允许的最大长度
#define GoE_NAMED_TYPE_FIELD_NAME_TOO_LONG                  0xFFFF1097  // 自定义类型的field 名字超过了允许的最大长度
#define GoE_INVALID_NAMED_TYPE_FIELD_LENGTH                 0xFFFF1098  // 无效的自定义数据类型的字段长度
#define GoE_INVALID_SEARCH_MASK                             0xFFFF1099  // 无效的高级搜索的标签点属性mask
#define GoE_RECYCLED_SPACE_NOT_ENOUGH                       0xFFFF109A  // 标签点回收站空闲空间不足
#define GoE_DYNAMIC_LOADED_MEMORY_NOT_INIT                  0xFFFF109B  // 动态加载的内存未初始化
#define GoE_FORBID_DYNAMIC_ALLOC_TYPE                       0xFFFF109C  // 内存库禁止动态分配类型
#define GoE_MEMORYDB_INDEX_CREATE_FAILED                    0xFFFF109D  // 内存库索引创建失败
#define GoE_WG_MAKE_QUERY_RETURN_NULL                       0xFFFF109F  //whitedb make_query_rc返回null
#define GoE_THTREAD_POOL_CREATED_FAILED                     0xFFFF10A0  // 内存库创建线程池失败
#define GoE_MEMORYDB_REMOVE_RECORD_FAILED                   0xFFFF10A1  //内存库删除记录失败
#define GoE_MEMORYDB_CONFIG_LOAD_FAILED                     0xFFFF10A2  // 内存库配置文件加载失败
#define GoE_MEMORYDB_PROHIBIT_DYNAMIC_ALLO_TYPE             0xFFFF10A3   //内存库禁止动态分配类型
#define GoE_MEMORYDB_DYNAMIC_ALLOC_TYPE_FAILED              0xFFFF10A4   //内存库动态分配类型失败
#define GoE_MEMORYDB_STORAGE_FILE_NAME_PARSE_FAILED         0xFFFF10A5   //内存库优先级文件名解析失败
#define GoE_MEMORYDB_TTREE_INDEX_DAMAGE                     0xFFFF10A6   //内存库T树索引损坏
#define GoE_MEMORYDB_CONFIG_FAILED                          0xFFFF10A7   //内存库配置文件错误
#define GoE_MEMORYDB_VALUE_COUNT_NOT_MATCH                  0xFFFF10A8   //内存库记录的值个数不匹配。
#define GoE_MEMORYDB_FIELD_TYPE_NOT_MATCH                   0xFFFF10A9   //内存库的字段类型不匹配       
#define GoE_MEMORYDB_MEMORY_ALLOC_FAILED                    0xFFFF10AA   //内存库内存分配失败
#define GoE_MEMORYDB_METHOD_PARAM_ERR                       0xFFFF10AB   //内存库方法参数错误
#define GoE_MEMORYDB_QUERY_RESULT_ALLOC_FAILED              0xFFFF10AC   //内存库查询结果缓存分配失败
#define GoE_FILE_PATH_LENGTH                                0xFFFF10AD   //指定的文件路径长度错误
#define GoE_MEMORYDB_FILE_VERSION_MATCH                     0xFFFF10AE   //内存库文件版本不匹配
#define GoE_MEMORYDB_FILE_CRC_ERROR                         0xFFFF10AF   //内存库文件CRC错误
#define GoE_MEMORYDB_FILE_FLAG_MATCH                        0xFFFF10B0   //内存库文件标志错误
#define GoE_MEMORYDB_INEXISTENCE                            0xFFFF10B1   //存储库不存在
#define GoE_MEMORYDB_LOAD_FAILED                            0xFFFF10B2   //存储库加载失败
#define GoE_NO_DATA_IN_INTERVAL                             0xFFFF10B3   //指定的查询区间内没有数据。
#define GoE_FILE_CREATED_FAIELD                             0xFFFF1004   //文件创建失败
#define GoE_FILE_FLUSH_FAILED                               0xFFFF1005   //文件刷新失败
#define GoE_INVALID_WRITE_TYPE                              0xFFFF1006
#define GoE_MEMORYDB_WAIT_OUT                               0xFFFF1007   //内存库等待超时

#define GoE_CANT_LOAD_MEMORYDB                              0xFFFF10B4  // 不能与内存服务取得联系
#define GoE_QUERY_IN_WHITEDB                                0xFFFF10B5  // 查询内存库过程中出现了错误，这是whitedb内部错误
#define GoE_NO_DATABASE_MEMORYDB                            0xFFFF10B6  //　没有找到指定数据类型所对应的分库
#define GoE_RECORD_NOT_GET                                  0xFFFF10B7  //  从whitedb中获取记录失败
#define GoE_MEMORY_ALLOC_ERR                                0xFFFF10B8  //  内存库用于接收快照的缓冲区分配失败
#define GoE_EVENT_CREATE_FAILED                             0xFFFF10B9  //  用于内存库接收缓冲区的事件创建失败
#define GoE_GET_POINT_FAILED                                0xFFFF10BA  //  获取标签点失败
#define GoE_MEMORY_INIT_FAILED                              0xFFFF10BB  //  内存库初始化失败
#define GoE_DATATYPE_NOT_MATCH                              0xFFFF10BC  //  数据类型不匹配
#define GoE_GET_FIELD_ERR                                   0xFFFF10BD  //  在whitedb获取记录的字段时出现了错误
#define GoE_MEMORYDB_INTERNAL_ERR                           0xFFFF10BE  //  whitedb内部未知错误
#define GoE_MEMORYDB_RECORD_CREATED_FAILED                  0xFFFF10BF  //  内存库创建记录失败
#define GoE_PARSE_NORMAL_TYPE_SNAPSHOT_ERR                  0xFFFF10C0  //  解析普通数据类型的快照失败
#define GoE_PARSE_NAMED_TYPE_SNAPSHOT_ERR                   0xFFFF10C1  //  解析自定义数据类型的快照失败
#define GoE_STRING_BLOB_TYPE_UNSUPPORT_CALC_POINT           0xFFFF10C2  // string、blob类型不支持计算点
#define GoE_COOR_TYPE_UNSUPPORT_CALC_POINT                  0xFFFF10C3  // 坐标类型不支持计算点
#define GoE_INCLUDE_HIS_DATA                                0xFFFF10C4  // 记录是历史数据，可能是无效过期的脏数据
#define GoE_THREAD_CREATE_ERR                               0xFFFF10C5  // 线程创建失败
#define GoE_XML_CRC_ERROR                                   0xFFFF10C6  // xml文件crc校验失败
#define GoE_OVERSIZE_INTERVALS                              0xFFFF10C7  // intervals >= system.ini配置的大小
#define GoE_DATETIMES_MUST_ASCENDING_ORDER                  0xFFFF10C8  // 时间必须按升序排序
#define GoE_CANT_LOAD_PERF                                  0xFFFF10C9  // 不能同性能计数服务取得联系
#define GoE_PERF_TAG_NOT_FOUND                              0xFFFF10CA  // 性能计数点不存在
#define GoE_WAIT_DATA_EMPTY                                 0xFFFF10CB  //  数据为空
#define GoE_WAIT_DATA_FULL                                  0xFFFF10CC  //  数据满了
#define GoE_DATA_TYPE_COUNT_LESS                            0xFFFF10CD  //
#define GoE_MEMORYDB_CREATE_FAILED                          0xFFFF10CE  //内存库创建失败
#define GoE_MEMORYDB_FIELD_ENCODE_FAILED                    0xFFFF10CF  //内存库字段编码失败
#define GoE_RECORD_CREATE_FAILED                            0xFFFF10D0  //内存库记录创建失败
#define GoE_REMOVE_RECORD_ERR                               0xFFFF10D1  //内存库记录删除失败
#define GoE_MEMORYDB_FILE_OPEN_FIELD                        0xFFFF10D2  //内存库打开文件失败
#define GoE_MEMORYDB_FILE_WRITE_FAILED                      0xFFFF10D3  //内存库写入文件失败
#define GoE_FILTER_WTIH_FLOAT_AND_EQUAL                     0xFFFF10D4  //含有浮点数不等式中不能有=号
#define GoE_DISPATCH_PLUGIN_NOT_EXSIT                       0xFFFF10D5  //转发服务器插件不存在
#define GoE_DISPATCH_PLUGIN_FILE_NOT_EXSIT					0xFFFF10D6  //转发服务器插件DLL文件不存在
#define GoE_DISPATCH_PLUGIN_ALREADY_EXSIT					0xFFFF10D7  //转发服务器插件已存在
#define GoE_DISPATCH_REGISTER_PLUGIN_FAILURE				0xFFFF10D8  //插件注册失败
#define GoE_DISPATCH_START_PLUGIN_FAILURE                   0xFFFF10D9  //启动插件失败
#define GoE_DISPATCH_STOP_PLUGIN_FAILURE                    0xFFFF10DA  //停止插件失败
#define GoE_DISPATCH_SET_PLUGIN_ENABLE_STATUS_FAILURE       0xFFFF10DB  //设置插件状态失败
#define GoE_DISPATCH_GET_PLUGIN_COUNT_FAILURE               0xFFFF10DC  //获取插件个数信息失败
#define GoE_DISPATCH_CONFIGFILE_NOT_EXIST					0xFFFF10DD    //转发服务配置文件不存在
#define GoE_DISPATCH_CONFIG_DATA_PARSE_ERR					0xFFFF10DE    //转发服务配置数据解析错误
#define GoE_DISPATCH_PLUGIN_ALREADY_RUNNING					0xFFFF10DF    //转发服务器插件已经运行
#define GoE_DISPATCH_PLUGIN_CANNOT_RUN						0xFFFF10E0    //转发服务器插件禁止运行
#define GoE_DISPATCH_PLUGIN_CONTAINER_UNRUN					0xFFFF10E1    //转发服务器插件容器未运行
#define GoE_DISPATCH_PLUGIN_INTERFACE_ERR					0xFFFF10E2    //转发服务器插件接口未实现
#define GoE_DISPATCH_PLUGIN_SAVE_CONFIG_ERR					0xFFFF10E3    //转发服务器保存配置文件出错
#define GoE_DISPATCH_PLUGIN_START_ERR						0xFFFF10E4	//转发服务器插件启动时失败
#define GoE_DISPATCH_PLUGIN_STOP_ERR						0xFFFF10E5	//转发服务器插件停止时失败
#define GoE_DISPATCH_PARSE_DATA_PAGE_ERR					0xFFFF10E6  //不支持的数据页类型
#define GoE_DISPATCH_NOT_RUN								0xFFFF10E7  //转发服务未启用

#define GoE_PROTOCOLNOTIMPL                                 0xFFFF2001  // 用户请求的报文未实现
#define GoE_CRCERROR                                        0xFFFF2002  // 报文CRC校验错误
#define GoE_WRONG_USERPW                                    0xFFFF2003  // 验证用户名密码失败
#define GoE_CHANGE_USERPW                                   0xFFFF2004  // 修改用户名密码失败
#define GoE_INVALID_HANDLE                                  0xFFFF2005  // 无效的句柄
#define GoE_INVALID_SOCKET_HANDLE                           0xFFFF2006  // 无效的套接字句柄
#define GoE_FALSE                                           0xFFFF2007  // 操作未成功完成，具体原因查看小错误码。
#define GoE_SCAN_POINT_NOT_FOUND                            0xFFFF2008  // 要求访问的采集标签点不存在或无效
#define GoE_CALC_POINT_NOT_FOUND                            0xFFFF2009  // 要求访问的计算标签点不存在或无效
#define GoE_REDUPLICATE_ID                                  0xFFFF200A  // 重复的标签点标识
#define GoE_HANDLE_SUBSCRIBED                               0xFFFF200B  // 句柄已经被订阅
#define GoE_OTHER_SDK_DOING                                 0xFFFF200C  // 另一个API正在执行
#define GoE_BATCH_END                                       0xFFFF200D  // 分段数据返回结束
#define GoE_AUTH_NOT_FOUND                                  0xFFFF200E  // 信任连接段不存在
#define GoE_AUTH_EXIST                                      0xFFFF200F  // 连接地址段已经位于信任列表中
#define GoE_AUTH_FULL                                       0xFFFF2010  // 信任连接段已满
#define GoE_USER_FULL                                       0xFFFF2011  // 用户已满
#define GoE_VERSION_UNMATCH                                 0xFFFF2012  // 报文或数据版本不匹配
#define GoE_INVALID_PRIV                                    0xFFFF2013  // 无效的权限
#define GoE_INVALID_MASK                                    0xFFFF2014  // 无效的子网掩码
#define GoE_INVALID_USERNAME                                0xFFFF2015  // 无效的用户名
#define GoE_INVALID_MARK                                    0xFFFF2016  // 无法识别的报文头标记
#define GoE_UNEXPECTED_METHOD                               0xFFFF2017  // 意外的消息 ID
#define GoE_INVALID_PARAM_INDEX                             0xFFFF2018  // 无效的系统参数索引值
#define GoE_DECODE_PACKET_ERROR                             0xFFFF2019  // 解包错误
#define GoE_ENCODE_PACKET_ERROR                             0xFFFF201A  // 编包错误
#define GoE_BLACKLIST_FULL                                  0xFFFF201B  // 阻止连接段已满
#define GoE_BLACKLIST_EXIST                                 0xFFFF201C  // 连接地址段已经位于黑名单中
#define GoE_BLACKLIST_NOT_FOUND                             0xFFFF201D  // 阻止连接段不存在
#define GoE_IN_BLACKLIST                                    0xFFFF201E  // 连接地址位于黑名单中，被主动拒绝
#define GoE_INCREASE_FILE_FAILED                            0xFFFF201F  // 试图增大文件失败
#define GoE_RPC_INTERFACE_FAILED                            0xFFFF2020  // 远程过程接口调用失败
#define GoE_CONNECTION_FULL                                 0xFFFF2021  // 连接已满
#define GoE_ONE_CLIENT_CONNECTION_FULL                      0xFFFF2022  // 连接已达到单个客户端允许连接数的最大值
#define GoE_SERVER_CLUTTER_POOL_NOT_ENOUGH                  0xFFFF2023  // 网络数据交换空间不足
#define GoE_EQUATION_CLUTTER_POOL_NOT_ENOUGH                0xFFFF2024  // 实时方程式交换空间不足
#define GoE_NAMED_TYPE_NAME_LEN_ERROR                       0xFFFF2025  // 自定义类型的名称过长
#define GoE_NAMED_TYPE_LENGTH_NOT_MATCH                     0xFFFF2026  // 数值长度与自定义类型的定义不符
#define GoE_CAN_NOT_UPDATE_SUMMARY                          0xFFFF2027  // 无法更新卫星数据
#define GoE_TOO_MANY_ARVEX_FILE                             0xFFFF2028  // 附属文件太多，无法继续创建附属文件
#define GoE_NOT_SUPPORTED_FEATURE                           0xFFFF2029  // 测试版本，暂时不支持此功能
#define GoE_ENSURE_ERROR                                    0xFFFF2030  // 验证信息失败，详细信息请查看数据库日志
#define GoE_OPERATOR_IS_CANCEL                              0xFFFF2031  // 操作被取消
#define GoE_MSGBODY_REV_ERROR                               0xFFFF2032  // 报文体接收失败
#define GoE_UNCOMPRESS_FAILED							    0xFFFF2033  // 解压缩失败
#define GoE_COMPRESS_FAILED									0xFFFF2034  // 压缩失败
#define GoE_SUBSCRIBE_ERROR                                 0xFFFF2035  // 订阅失败，前一个订阅线程尚未退出，请稍后重试
#define GoE_SUBSCRIBE_CANCEL_ERROR                          0xFFFF2036  // 取消订阅失败
#define GoE_SUBSCRIBE_CALLBACK_FAILED                       0xFFFF2037  // 订阅回掉函数中不能调用取消订阅、断开连接
#define GoE_SUBSCRIBE_GREATER_MAX_COUNT                     0xFFFF2038  // 超过单连接可订阅标签点数量



// Windows Socket Errors
#define GoE_NET_ERROR                                       0xFFFF7000  // 网络错误的起始值
#define GoE_SOCK_WSAEINTR                                   0xFFFF7004  // （阻塞）调用被 WSACancelBlockingCall() 函数取消
#define GoE_SOCK_WSAEACCES                                  0xFFFF700D  // 请求地址是广播地址，但是相应的 flags 没设置
#define GoE_SOCK_WSAEFAULT                                  0xFFFF700E  // 非法内存访问
#define GoE_SOCK_WSAEMFILE                                  0xFFFF7018  // 无多余的描述符可用
#define GoE_SOCK_WSAEWOULDBLOCK                             0xFFFF7023  // 套接字被标识为非阻塞，但操作将被阻塞
#define GoE_SOCK_WSAEINPROGRESS                             0xFFFF7024  // 一个阻塞的 Windows Sockets 操作正在进行
#define GoE_SOCK_WSAEALREADY                                0xFFFF7025  // 一个非阻塞的 connect() 调用已经在指定的套接字上进行
#define GoE_SOCK_WSAENOTSOCK                                0xFFFF7026  // 描述符不是套接字描述符
#define GoE_SOCK_WSAEDESTADDRREQ                            0xFFFF7027  // 要求（未指定）目的地址
#define GoE_SOCK_WSAEMSGSIZE                                0xFFFF7028  // 套接字为基于消息的，消息太大（大于底层传输支持的最大值）
#define GoE_SOCK_WSAEPROTOTYPE                              0xFFFF7029  // 对此套接字来说，指定协议是错误的类型
#define GoE_SOCK_WSAEPROTONOSUPPORT                         0xFFFF702B  // 不支持指定协议
#define GoE_SOCK_WSAESOCKTNOSUPPORT                         0xFFFF702C  // 在此地址族中不支持指定套接字类型
#define GoE_SOCK_WSAEOPNOTSUPP                              0xFFFF702D  // MSG_OOB 被指定，但是套接字不是流风格的
#define GoE_SOCK_WSAEAFNOSUPPORT                            0xFFFF702F  // 不支持指定的地址族
#define GoE_SOCK_WSAEADDRINUSE                              0xFFFF7030  // 套接字的本地地址已被使用
#define GoE_SOCK_WSAEADDRNOTAVAIL                           0xFFFF7031  // 远程地址非法
#define GoE_SOCK_WSAENETDOWN                                0xFFFF7032  // Windows Sockets 检测到网络系统已经失效
#define GoE_SOCK_WSAENETUNREACH                             0xFFFF7033  // 网络无法到达主机
#define GoE_SOCK_WSAENETRESET                               0xFFFF7034  // 在操作进行时 keep-alive 活动检测到一个失败，连接被中断
#define GoE_SOCK_WSAECONNABORTED                            0xFFFF7035  // 连接因超时或其他失败而中断
#define GoE_SOCK_WSAECONNRESET                              0xFFFF7036  // 连接被复位
#define GoE_SOCK_WSAENOBUFS                                 0xFFFF7037  // 无缓冲区空间可用
#define GoE_SOCK_WSAEISCONN                                 0xFFFF7038  // 连接已建立
#define GoE_SOCK_WSAENOTCONN                                0xFFFF7039  // 套接字未建立连接
#define GoE_SOCK_WSAESHUTDOWN                               0xFFFF703A  // 套接字已 shutdown，连接已断开
#define GoE_SOCK_WSAETIMEDOUT                               0xFFFF703C  // 连接请求超时，未能建立连接
#define GoE_SOCK_WSAECONNREFUSED                            0xFFFF703D  // 连接被拒绝
#define GoE_SOCK_WSAECLOSE                                  0xFFFF703E  // 连接被关闭
#define GoE_SOCK_WSANOTINITIALISED                          0xFFFF705D  // Windows Sockets DLL 未初始化

// C error number from errno
#define GoE_C_ERRNO_ERROR                                   0xFFFF8000  // C语言errno错误的起始值
#define GoE_C_ERRNO_EPERM                                   0xFFFF8001  // operation not permitted
#define GoE_C_ERRNO_ENOENT                                  0xFFFF8002  // no such file or directory
#define GoE_C_ERRNO_ESRCH                                   0xFFFF8003  // no such process
#define GoE_C_ERRNO_EINTR                                   0xFFFF8004  // interrupted
#define GoE_C_ERRNO_EIO                                     0xFFFF8005  // io error
#define GoE_C_ERRNO_ENXIO                                   0xFFFF8006  // no such device or address
#define GoE_C_ERRNO_E2BIG                                   0xFFFF8007  // argument list too long
#define GoE_C_ERRNO_ENOEXEC                                 0xFFFF8008  // exec format error
#define GoE_C_ERRNO_EBADF                                   0xFFFF8009  // bad file descriptor
#define GoE_C_ERRNO_ECHILD                                  0xFFFF800A  // no child process
#define GoE_C_ERRNO_EAGAIN                                  0xFFFF800B  // resource unavailable try again
#define GoE_C_ERRNO_ENOMEM                                  0xFFFF800C  // not enough memory
#define GoE_C_ERRNO_EACCES                                  0xFFFF800D  // permission denied
#define GoE_C_ERRNO_EFAULT                                  0xFFFF800E  // bad address
#define GoE_C_ERRNO_EBUSY                                   0xFFFF8010  // device or resource busy
#define GoE_C_ERRNO_EEXIST                                  0xFFFF8011  // file exists
#define GoE_C_ERRNO_EXDEV                                   0xFFFF8012  // cross device link
#define GoE_C_ERRNO_ENODEV                                  0xFFFF8013  // no such device
#define GoE_C_ERRNO_ENOTDIR                                 0xFFFF8014  // not a directory
#define GoE_C_ERRNO_EISDIR                                  0xFFFF8015  // is a directory
#define GoE_C_ERRNO_EINVAL                                  0xFFFF8016  // invalid argument
#define GoE_C_ERRNO_ENFILE                                  0xFFFF8017  // too many files open in system
#define GoE_C_ERRNO_EMFILE                                  0xFFFF8018  // too many files open
#define GoE_C_ERRNO_ENOTTY                                  0xFFFF8019  // inappropriate io control operation
#define GoE_C_ERRNO_EFBIG                                   0xFFFF801A  // file too large
#define GoE_C_ERRNO_ENOSPC                                  0xFFFF801B  // no space on device
#define GoE_C_ERRNO_ESPIPE                                  0xFFFF801C  // invalid seek
#define GoE_C_ERRNO_EROFS                                   0xFFFF801D  // read only file system
#define GoE_C_ERRNO_EMLINK                                  0xFFFF801F  // too many links
#define GoE_C_ERRNO_EPIPE                                   0xFFFF8020  // broken pipe
#define GoE_C_ERRNO_EDOM                                    0xFFFF8021  // argument out of domain
#define GoE_C_ERRNO_ERANGE                                  0xFFFF8022  // result out of range
#define GoE_C_ERRNO_EDEADLK                                 0xFFFF8024  // resource deadlock would occur
#define GoE_C_ERRNO_ENAMETOOLONG                            0xFFFF8026  // filename too long
#define GoE_C_ERRNO_ENOLCK                                  0xFFFF8027  // no lock available
#define GoE_C_ERRNO_ENOSYS                                  0xFFFF8028  // function not supported
#define GoE_C_ERRNO_ENOTEMPTY                               0xFFFF8029  // directory not empty

#define GoE_C_ERRNO_EILSEQ                                  0xFFFF802A  // illegal byte sequence
#define GoE_C_ERRNO_STRUNCATE                               0xFFFF8050  // a string copy of concatenation resulted in a truncated string

/* POSIXGoE_C_ERRNO_ESUPPLEMENT */
#define GoE_C_ERRNO_EADDRINUSE                              0xFFFF8064  // address in use
#define GoE_C_ERRNO_EADDRNOTAVAIL                           0xFFFF8065  // address not available
#define GoE_C_ERRNO_EAFNOSUPPORT                            0xFFFF8066  // address family not supported
#define GoE_C_ERRNO_EALREADY                                0xFFFF8067  // connection already in progress
#define GoE_C_ERRNO_EBADMSG                                 0xFFFF8068  // bad message
#define GoE_C_ERRNO_ECANCELED                               0xFFFF8069  // operation canceled
#define GoE_C_ERRNO_ECONNABORTED                            0xFFFF806A  // connection aborted
#define GoE_C_ERRNO_ECONNREFUSED                            0xFFFF806B  // connection refused
#define GoE_C_ERRNO_ECONNRESET                              0xFFFF806C  // connection reset
#define GoE_C_ERRNO_EDESTADDRREQ                            0xFFFF806D  // destination address required
#define GoE_C_ERRNO_EHOSTUNREACH                            0xFFFF806E  // host unreachable
#define GoE_C_ERRNO_EIDRM                                   0xFFFF806F  // identifier removed
#define GoE_C_ERRNO_EINPROGRESS                             0xFFFF8070  // operation in progress
#define GoE_C_ERRNO_EISCONN                                 0xFFFF8071  // already connected
#define GoE_C_ERRNO_ELOOP                                   0xFFFF8072  // too many synbolic link levels
#define GoE_C_ERRNO_EMSGSIZE                                0xFFFF8073  // message size
#define GoE_C_ERRNO_ENETDOWN                                0xFFFF8074  // network down
#define GoE_C_ERRNO_ENETRESET                               0xFFFF8075  // network reset
#define GoE_C_ERRNO_ENETUNREACH                             0xFFFF8076  // network unreachable
#define GoE_C_ERRNO_ENOBUFS                                 0xFFFF8077  // no buffer space
#define GoE_C_ERRNO_ENODATA                                 0xFFFF8078  // no message available
#define GoE_C_ERRNO_ENOLINK                                 0xFFFF8079  // no link
#define GoE_C_ERRNO_ENOMSG                                  0xFFFF807A  // no message
#define GoE_C_ERRNO_ENOPROTOOPT                             0xFFFF807B  // no protocol option
#define GoE_C_ERRNO_ENOSR                                   0xFFFF807C  // no stream resources
#define GoE_C_ERRNO_ENOSTR                                  0xFFFF807D  // not a stream
#define GoE_C_ERRNO_ENOTCONN                                0xFFFF807E  // not connected
#define GoE_C_ERRNO_ENOTRECOVERABLE                         0xFFFF807F  // state not recoverable
#define GoE_C_ERRNO_ENOTSOCK                                0xFFFF8080  // not a socket
#define GoE_C_ERRNO_ENOTSUP                                 0xFFFF8081  // not supported
#define GoE_C_ERRNO_EOPNOTSUPP                              0xFFFF8082  // operation not supported
#define GoE_C_ERRNO_EOTHER                                  0xFFFF8083  // other
#define GoE_C_ERRNO_EOVERFLOW                               0xFFFF8084  // value too large
#define GoE_C_ERRNO_EOWNERDEAD                              0xFFFF8085  // owner dead
#define GoE_C_ERRNO_EPROTO                                  0xFFFF8086  // protocol error
#define GoE_C_ERRNO_EPROTONOSUPPORT                         0xFFFF8087  // protocol not supported
#define GoE_C_ERRNO_EPROTOTYPE                              0xFFFF8088  // wrong protocol type
#define GoE_C_ERRNO_ETIME                                   0xFFFF8089  // stream timeout
#define GoE_C_ERRNO_ETIMEDOUT                               0xFFFF808A  // timed out
#define GoE_C_ERRNO_ETXTBSY                                 0xFFFF808B  // text file busy
#define GoE_C_ERRNO_EWOULDBLOCK                             0xFFFF808C  // operation would block

#define GoE_IPC_ERROR                                       0xFFFFC000  // ipc error begin
#define GoE_IPC_ERROR_END                                   0xFFFFC09F  // ipc error end

#endif /* __GOLDEN_ERROR_H__ */
