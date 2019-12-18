#ifndef __GOLDEN_API_H__
#define __GOLDEN_API_H__

#include "golden.h"
#include "golden_error.h" 

/************************************ API 配置接口 ************************************/

/**
* 命名：go_get_api_version 
* 功能：取得 goldenapi 库的版本号
* 参数：
*      [major]        整型，输出，主版本号
*      [minor]        整型，输出，次版本号
*      [beta]         整型，输出，发布版本号
* 备注：如果返回的版本号与 golden.h 中定义的不匹配(GOLDEN_API_XXX_VERSION)，则应用程序使用了错误的库。
*      应输出一条错误信息并退出，否则可能在调用某些 api 时会导致崩溃。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_api_version(
                   golden_int32 *major,
                   golden_int32 *minor, 
                   golden_int32 *beta
                   );

/**
* 命名：go_set_option 
* 功能：配置 api 行为参数
* 参数：
*      [type]        整型，输入，选项类别，参见枚举 GOLDEN_API_OPTION
*      [value]       整型，输入，选项值。
* 备注：选项设置后在下一次调用 api 时才生效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_set_option(
              golden_int32 type,
              golden_int32 value 
              );

GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  go_create_datagram_handle(
  golden_int32 port,
  const char* remotehost,
  golden_datagram_handle& handle
  );

GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  go_remove_datagram_handle(
  golden_datagram_handle handle
  );

GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  go_recv_datagram(
  char* message,
  golden_int32& message_len,
  golden_datagram_handle handle,
  char* remote_addr,
  golden_int32 timeout=-1
  );

/************************************ 网络服务接口 ************************************/

GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  go_subscribe_connect(
  golden_int32 handle,
  go_connect_event callback
  );

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_subscribe_connect_ex(
  golden_int32 handle,
  golden_uint32 options,
  void* param,
  go_connect_event_ex callback
);

GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  go_cancel_subscribe_connect(
  golden_int32 handle
  );

/**
* 命名：go_connect
* 功能：建立同 GOLDEN 数据库的网络连接
* 参数：
*      [hostname]     字符串，输入，GOLDEN 数据平台服务器的网络地址或机器名
*      [port]         整型，输入，缺省值 6327，连接端口
*      [handle]       整型，输出，连接句柄
* 备注：在调用所有的接口函数之前，必须先调用本函数建立同Golden服务器的连接。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_connect(
           const char *hostname, 
           golden_int32 port,
           golden_int32 *handle
           );

/**
* 命名：go_connection_count
* 功能：返回 GOLDEN 服务器当前连接个数
* 参数：
*      [handle]   连接句柄
*      [count]    整型，输出，返回当前主机连接个数
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_connection_count(
                    golden_int32 handle, 
                    golden_int32 *count
                    );
/**
* 命名：go_get_connections
* 功能：列出 GOLDEN 服务器的所有连接句柄
* 参数：
*      [handle]    连接句柄
*      [sockets]   整型数组，输出，所有连接的套接字句柄
*      [count]     整型，输入/输出，输入表示sockets的长度，输出表示返回的连接个数
* 备注：用户须保证分配给 sockets 的空间与 count 相符
*      如果输入的 count 小于输出的 count，则只返回部分连接
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_connections(
                   golden_int32 handle,
                   golden_int32 *sockets ,
                   golden_int32 *count
                   );
/**
* 命名：go_get_own_connection
* 功能：获取当前连接的socket句柄
* 参数：
*      [handle]    连接句柄
*      [socket]   整型，输出，当前连接的套接字句柄
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_get_own_connection(
                golden_int32 handle,
                golden_int32* socket
                );
/**
* 命名：go_get_connection_info
* 功能：返回 GOLDEN 服务器指定连接的信息
* 参数：
*      [handle]  连接句柄
*      [socket]  整型，输入，指定的连接
*      [info]    GOLDEN_HOST_CONNECT_INFO 结构，输入，与连接相关的信息
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_connection_info(
                       golden_int32 handle, 
                       golden_int32 socket,
                       GOLDEN_HOST_CONNECT_INFO *info
                       );

/**
* 命名：go_disconnect
* 功能：断开同 GOLDEN 数据平台的连接
* 参数：
*      [handle]   连接句柄
* 备注：完成对 GOLDEN 的访问后调用本函数断开连接。
*      连接一旦断开，则需要重新连接后才能调用其他的接口函数。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_disconnect(
              golden_int32 handle
              );

/**
* 命名：go_login
* 功能：以有效帐户登录
* 参数：
*      [handle]    连接句柄
*      [user]      字符串，输入，登录帐户
*      [password]  字符串，输入，帐户口令
*      [priv]      整型，GOLDEN_RO, GOLDEN_DW, GOLDEN_TA, GOLDEN_SA 之一，输出，帐户权限
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_login(
         golden_int32 handle,
         const char *user, 
         const char *password, 
         golden_int32 *priv
         );

/**
* 命名：go_get_linked_ostype
* 功能：获取连接句柄所连接的服务器操作系统类型
* 参数：
*      [handle]           连接句柄
*      [Golden_OS_Type]   操作系统类型
* 备注：如句柄未链接任何服务器，返回GOLDEN_OS_INVALID(当前支持操作系统类型：windows、linux)。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_get_linked_ostype(
                     golden_int32 handle,
                     Golden_OS_Type* ostype
                    );

/**
* 命名：go_change_password
* 功能：修改用户帐户口令
* 参数：
*      [handle]        连接句柄
*      [user]          字符串，输入，已有帐户
*      [password]      字符串，输入，帐户新口令
* 备注：只有系统管理员可以修改其它用户的密码。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_change_password(
                   golden_int32 handle,
                   const char *user, 
                   const char *password
                   );

/**
* 命名：go_change_my_password
* 功能：用户修改自己帐户口令
* 参数：
*      [handle]        连接句柄
*      [old_pwd]       字符串，输入，帐户原口令
*      [new_pwd]       字符串，输入，帐户新口令
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_change_my_password(
                      golden_int32 handle,
                      const char *old_pwd, 
                      const char *new_pwd
                      );


/**
* 命名：go_get_priv
* 功能：获取连接权限
* 参数：
*      [handle]    连接句柄
*      [priv]      整型，GOLDEN_RO, GOLDEN_DW, GOLDEN_TA, GOLDEN_SA 之一，输出，帐户权限
* 备注：如果还未登陆或不在服务器信任连接中，对应权限为-1，表示没有任何权限。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_priv(
            golden_int32 handle,
            golden_int32 *priv
            );


/**
* 命名：go_change_priv
* 功能：修改用户帐户权限
* 参数：
*      [handle]    连接句柄
*      [user]      字符串，输入，已有帐户
*      [priv]      整型，GOLDEN_RO, GOLDEN_DW, GOLDEN_TA, GOLDEN_SA 之一，输入，帐户权限
* 备注：只有管理员有修改权限
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_change_priv(
               golden_int32 handle,
               const char *user, 
               golden_int32 priv
               );

/**
* 命名：go_add_user
* 功能：添加用户帐户
* 参数：
*      [handle]    连接句柄
*      [user]      字符串，输入，帐户
*      [password]  字符串，输入，帐户初始口令
*      [priv]      整型，GOLDEN_RO, GOLDEN_DW, GOLDEN_TA, GOLDEN_SA 之一，输入，帐户权限
* 备注：只有管理员有添加用户权限
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_add_user(
            golden_int32 handle,
            const char *user,
            const char *password,
            golden_int32 priv
            );

/**
* 命名：go_remove_user
* 功能：删除用户帐户
* 参数：
*      [handle]    连接句柄
*      [user]      字符串，输入，帐户名
* 备注：只有管理员有删除用户权限
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_remove_user(
               golden_int32 handle,
               const char *user
               );

/**
* 命名：go_lock_user
* 功能：启用或禁用用户
* 参数：
*      [handle]    连接句柄
*      [user]      字符串，输入，帐户名
*      [lock]      布尔，输入，是否禁用
* 备注：只有管理员有启用禁用权限
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_lock_user(
golden_int32 handle,
const char *user,
bool lock
);


/**
* 命名：go_get_users
* 功能：获得所有用户
* 参数：
*      [handle]    连接句柄
*      [count]     整型，输入/输出，输入时表示 users、privs 的长度，即用户个数；
*                  输出时表示成功返回的用户信息个数。
*      [info]      结构体数组指针，输出，用户信息
* 备注：用户须保证分配给 info 的空间与 count 相符，
*       如果输入的 count 小于总的用户数，则只返回部分用户信息。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_users(
             golden_int32 handle,
             golden_int32 *count,
             GOLDEN_USER_INFO *infos
             );


/**
* 命名：go_add_blacklist
* 功能：添加连接黑名单项
* 参数：
*      [handle]    连接句柄
*      [addr]      字符串，输入，阻止连接段地址。
*      [mask]      字符串，输入，阻止连接段子网掩码。
*      [desc]      字符串，输入，阻止连接段的说明，超过 511 字符将被截断。
* 备注：addr 和 mask 进行与运算形成一个子网，
*      来自该子网范围内的连接都将被阻止，黑名单的优先级高于信任连接，
*      如果有连接既位于黑名单中，也位于信任连接中，则它将先被阻止。
*      有效的子网掩码的所有 1 位于 0 左侧，例如："255.255.254.0"。
*      当全部为 1 时，表示该子网中只有 addr 一个地址；但不能全部为 0。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_add_blacklist(
                 golden_int32 handle,
                 const char *addr, 
                 const char *mask, 
                 const char *desc
                 );


/**
* 命名：go_update_blacklist
* 功能：更新连接连接黑名单项
* 参数：
*      [handle]    连接句柄
*      [addr]      字符串，输入，原阻止连接段地址。
*      [mask]      字符串，输入，原阻止连接段子网掩码。
*      [addr_new]  字符串，输入，新的阻止连接段地址。
*      [mask_new]  字符串，输入，新的阻止连接段子网掩码。
*      [desc]      字符串，输入，新的阻止连接段的说明，超过 511 字符将被截断。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_update_blacklist(
                    golden_int32 handle,
                    const char *addr, 
                    const char *mask, 
                    const char *addr_new, 
                    const char *mask_new, 
                    const char *desc
                    );

/**
* 命名：go_remove_blacklist
* 功能：删除连接黑名单项
* 参数：
*      [handle]    连接句柄
*      [addr]      字符串，输入，阻止连接段地址。
*      [mask]      字符串，输入，阻止连接段子网掩码。
* 备注：只有 addr 与 mask 完全相同才视为同一个阻止连接段。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_remove_blacklist(
                    golden_int32 handle,
                    const char *addr, 
                    const char *mask
                    );

/**
* 命名：go_get_blacklist
* 功能：获得连接黑名单
* 参数：
*      [handle]    连接句柄
*      [addrs]     字符串指针数组，输出，阻止连接段地址列表
*      [masks]     字符串指针数组，输出，阻止连接段子网掩码列表
*      [descs]     字符串指针数组，输出，阻止连接段的说明。
*      [count]     整型，输入/输出，用户个数
* 备注：用户须保证分配给 addrs, masks, descs 的空间与 count 相符，
*      如果输入的 count 小于输出的 count，则只返回部分阻止连接段，
*      addrs, masks 中每个字符串指针所指缓冲区尺寸不得小于 32 字节，
*      descs 中每个字符串指针所指缓冲区尺寸不得小于 512 字节。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_blacklist(
                 golden_int32 handle,
                 char* const* addrs,
                 char* const* masks, 
                 char* const* descs, 
                 golden_int32 *count
                 );


/**
* 命名：go_add_authorization
* 功能：添加信任连接段
* 参数：
*      [handle]    连接句柄
*      [addr]      字符串，输入，信任连接段地址。
*      [mask]      字符串，输入，信任连接段子网掩码。
*      [priv]      整数，输入，信任连接段拥有的用户权限。
*      [desc]      字符串，输入，信任连接段的说明，超过 511 字符将被截断。
* 备注：addr 和 mask 进行与运算形成一个子网，
*        来自该子网范围内的连接都被视为可信任的，
*        可以不用登录 (go_login)，就直接调用 API， 
*        它所拥有的权限在 priv 中指定。
*        有效的子网掩码的所有 1 位于 0 左侧，
*        例如："255.255.254.0"。当全部为 1 时，
*        表示该子网中只有 addr 一个地址；
*        但不能全部为 0。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_add_authorization(
                     golden_int32 handle,
                     const char *addr, 
                     const char *mask, 
                     golden_int32 priv, 
                     const char *desc
                     );


/**
* 命名：go_update_authorization
* 功能：更新信任连接段
* 参数：
*      [handle]    连接句柄
*      [addr]      字符串，输入，原信任连接段地址。
*      [mask]      字符串，输入，原信任连接段子网掩码。
*      [addr_new]  字符串，输入，新的信任连接段地址。
*      [mask_new]  字符串，输入，新的信任连接段子网掩码。
*      [priv]      整数，输入，新的信任连接段拥有的用户权限。
*      [desc]      字符串，输入，新的信任连接段的说明，超过 511 字符将被截断。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_update_authorization(
                        golden_int32 handle,
                        const char *addr, 
                        const char *mask, 
                        const char *addr_new, 
                        const char *mask_new, 
                        golden_int32 priv, 
                        const char *desc
                        );

/**
* 命名：go_remove_authorization
* 功能：删除信任连接段
* 参数：
*      [handle]    连接句柄
*      [addr]      字符串，输入，信任连接段地址。
*      [mask]      字符串，输入，信任连接段子网掩码。
* 备注：只有 addr 与 mask 完全相同才视为同一个信任连接段。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_remove_authorization(
                        golden_int32 handle,
                        const char *addr, 
                        const char *mask
                        );

/**
* 命名：go_get_authorizations
* 功能：获得所有信任连接段
* 参数：
*      [handle]    连接句柄
*      [addrs]     字符串指针数组，输出，信任连接段地址列表
*      [masks]     字符串指针数组，输出，信任连接段子网掩码列表
*      [privs]     整型数组，输出，信任连接段权限列表
*      [descs]     字符串指针数组，输出，信任连接段的说明。
*      [count]     整型，输入/输出，用户个数
* 备注：用户须保证分配给 addrs, masks, privs, descs 的空间与 count 相符，
*        如果输入的 count 小于输出的 count，则只返回部分信任连接段，
*        addrs, masks 中每个字符串指针所指缓冲区尺寸不得小于 32 字节，
*        descs 中每个字符串指针所指缓冲区尺寸不得小于 512 字节。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_authorizations(
                      golden_int32 handle,
                      char* const* addrs,
                      char* const* masks, 
                      golden_int32 *privs, 
                      char* const* descs, 
                      golden_int32 *count
                      );
/**
* 命名：go_release_pointer
* 功能：释放从goldenapi中自动创建的char*指针
* 参数：
*      [handle]    连接句柄
*      [pointer]     字符串指针，输入，输出，输入时 *pointer 不能为空，输出时 *pointer为空
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_release_pointer(golden_int32 handle, char** pointer);

/**
* 命名：go_host_time
* 功能：获取 GOLDEN 服务器当前UTC时间
* 参数：
*      [handle]       连接句柄
*      [hosttime]     整型，输出，Golden服务器的当前UTC时间，
*                     表示距离1970年1月1日08:00:00的秒数。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_host_time(
             golden_int32 handle, 
             golden_int32 *hosttime
             );

/**
* 命名：go_format_timespan
* 功能：根据时间跨度值生成时间格式字符串
* 参数：
*      [str]          字符串，输出，时间格式字符串，形如: 
*                     "1d" 表示时间跨度为24小时。
*                     具体含义参见 go_parse_timespan 注释。
*      [timespan]     整型，输入，要处理的时间跨度秒数。
* 备注：字符串缓冲区大小不应小于 32 字节。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_format_timespan(
                   char *str, 
                   golden_int32 timespan
                   );

/**
* 命名：go_parse_timespan
* 功能：根据时间格式字符串解析时间跨度值
* 参数：
*      [str]          字符串，输入，时间格式字符串，规则如下：
*                     [time_span]
*
*                     时间跨度部分可以出现多次，
*                     可用的时间跨度代码及含义如下：
*                     ?y            ?年, 1年 = 365日
*                     ?m            ?月, 1月 = 30 日
*                     ?d            ?日
*                     ?h            ?小时
*                     ?n            ?分钟
*                     ?s            ?秒
*                     例如："1d" 表示时间跨度为24小时。
*
*      [timespan]     整型，输出，返回解析得到的时间跨度秒数。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_parse_timespan(
                  const char *str, 
                  golden_int32 *timespan
                  );

/**
* 命名：go_parse_time
* 功能：根据时间格式字符串解析时间值
* 参数：
*      [str]          字符串，输入，时间格式字符串，规则如下：
*                     base_time [[+|-] offset_time]
*
*                     其中 base_time 表示基本时间，有三种形式：
*                     1. 时间字符串，如 "2010-1-1" 及 "2010-1-1 8:00:00"；
*                     2. 时间代码，表示客户端相对时间；
*                     可用的时间代码及含义如下：
*                     td             当天零点
*                     yd             昨天零点
*                     tm             明天零点
*                     mon            本周一零点
*                     tue            本周二零点
*                     wed            本周三零点
*                     thu            本周四零点
*                     fri            本周五零点
*                     sat            本周六零点
*                     sun            本周日零点
*                     3. 星号('*')，表示客户端当前时间。
*
*                     offset_time 是可选的，可以出现多次，
*                     可用的时间偏移代码及含义如下：
*                     [+|-] ?y            偏移?年, 1年 = 365日
*                     [+|-] ?m            偏移?月, 1月 = 30 日
*                     [+|-] ?d            偏移?日
*                     [+|-] ?h            偏移?小时
*                     [+|-] ?n            偏移?分钟
*                     [+|-] ?s            偏移?秒
*                     [+|-] ?ms           偏移?毫秒
*                     例如："*-1d" 表示当前时刻减去24小时。
*
*      [datetime]     整型，输出，返回解析得到的时间值。
*      [ms]           短整型，输出，返回解析得到的时间毫秒值。
*  备注：ms 可以为空指针，相应的毫秒信息将不再返回。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_parse_time(
              const char *str, 
              golden_int64 *datetime, 
              golden_int16 *ms 
              );

/**
* 命名：go_format_message
* 功能：获取 Golden API 调用返回值的简短描述
* 参数：
*      [ecode]        无符号整型，输入，Golden API调用后的返回值，详见golden_error.h头文件
*      [message]      字符串，输出，返回错误码简短描述
*      [name]         字符串，输出，返回错误码宏名称
*      [size]         整型，输入，message 参数的字节长度
* 备注：用户须保证分配给 message， name 的空间与 size 相符, 
*      name 或 message 可以为空指针，对应的信息将不再返回。
*/
GOLDENAPI
void  
GOLDENAPI_CALLRULE 
go_format_message(
                  golden_error ecode, 
                  char *message, 
                  char *name, 
                  golden_int32 size
                  );

/**
* 命名：go_job_message
* 功能：获取任务的简短描述
* 参数：
*      [job_id]       整型，输入，GOLDEN_HOST_CONNECT_INFO::job 字段所表示的最近任务的描述
*      [desc]         字符串，输出，返回任务描述
*      [name]         字符串，输出，返回任务名称
*      [size]         整型，输入，desc、name 参数的字节长度
* 备注：用户须保证分配给 desc、name 的空间与 size 相符， 
*      name 或 message 可以为空指针，对应的信息将不再返回。
*/
GOLDENAPI
void  
GOLDENAPI_CALLRULE 
go_job_message(
               golden_int32 job_id, 
               char *desc,
               char *name, 
               golden_int32 size
               );

/**
* 命名：go_set_timeout
* 功能：设置连接超时时间
* 参数：
*      [handle]   连接句柄
*      [socket]   整型，输入，要设置超时时间的连接
*      [timeout]  整型，输入，超时时间，单位为秒，0 表示始终保持
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_set_timeout(
               golden_int32 handle,  
               golden_int32 socket ,
               golden_int32 timeout
               );

/**
* 命名：go_get_timeout
* 功能：获得连接超时时间
* 参数：
*      [handle]   连接句柄
*      [socket]   整型，输入，要获取超时时间的连接
*      [timeout]  整型，输出，超时时间，单位为秒，0 表示始终保持
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_timeout(
               golden_int32 handle, 
               golden_int32 socket,
               golden_int32 *timeout
               );

/**
* 命名：go_kill_connection
* 功能：断开已知连接
* 参数：
*      [handle]    连接句柄
*      [socket]    整型，输入，要断开的连接
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_kill_connection(
                   golden_int32 handle, 
                   golden_int32 socket
                   );

/**
* 命名：go_get_db_info1
* 功能：获得字符串型数据库系统参数
* 参数：
*      [handle]    连接句柄
*      [index]     整型，输入，要取得的参数索引，参见枚举 GOLDEN_DB_PARAM_INDEX。
*      [str]       字符串型，输出，存放取得的字符串参数值。
*      [size]      整型，输入，字符串缓冲区尺寸。
* 备注：本接口只接受 [GOLDEN_PARAM_STR_FIRST, GOLDEN_PARAM_STR_LAST) 范围之内参数索引。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_db_info1(
                golden_int32 handle,
                golden_int32 index, 
                char *str, 
                golden_int32 size
                );

/**
* 命名：go_get_db_info2
* 功能：获得整型数据库系统参数
* 参数：
*      [handle]    连接句柄
*      [index]     整型，输入，要取得的参数索引，参见枚举 GOLDEN_DB_PARAM_INDEX。
*      [value]     无符号整型，输出，存放取得的整型参数值。
* 备注：本接口只接受 [GOLDEN_PARAM_INT_FIRST, GOLDEN_PARAM_INT_LAST) 范围之内参数索引。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_db_info2(
                golden_int32 handle,
                golden_int32 index, 
                golden_uint32 *value
                );

/**
* 命名：go_set_db_info1
* 功能：设置字符串型数据库系统参数
* 参数：
*      [handle]    连接句柄
*      [index]     整型，输入，要设置的参数索引，参见枚举 GOLDEN_DB_PARAM_INDEX。
*                  其中，仅以下列出的枚举值可用：
*                  GOLDEN_PARAM_AUTO_BACKUP_PATH, 
*                  GOLDEN_PARAM_SERVER_SENDER_IP, 
*      [str]       字符串型，输入，新的参数值。
* 备注：如果修改了启动参数，将返回 GoE_DATABASE_NEED_RESTART 提示码。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_set_db_info1(
                golden_int32 handle,
                golden_int32 index, 
                const char *str
                );

/**
* 命名：go_set_db_info2
* 功能：设置整型数据库系统参数
* 参数：
*      [handle]    连接句柄
*      [index]     整型，输入，要取得的参数索引，参见枚举 GOLDEN_DB_PARAM_INDEX。
*                  其中，仅以下列出的枚举值可用：
*                  GOLDEN_PARAM_SERVER_IPC_SIZE, 
*                  GOLDEN_PARAM_EQUATION_IPC_SIZE, 
*                  GOLDEN_PARAM_HASH_TABLE_SIZE, 
*                  GOLDEN_PARAM_TAG_DELETE_TIMES, 
*                  GOLDEN_PARAM_SERVER_PORT, 
*                  GOLDEN_PARAM_SERVER_SENDER_PORT, 
*                  GOLDEN_PARAM_SERVER_RECEIVER_PORT, 
*                  GOLDEN_PARAM_SERVER_MODE, 
*                  GOLDEN_PARAM_ARV_PAGES_NUMBER, 
*                  GOLDEN_PARAM_ARVEX_PAGES_NUMBER, 
*                  GOLDEN_PARAM_EXCEPTION_AT_SERVER, 
*                  GOLDEN_PARAM_EX_ARCHIVE_SIZE, 
*                  GOLDEN_PARAM_ARCHIVE_BATCH_SIZE, 
*                  GOLDEN_PARAM_ARV_ASYNC_QUEUE_SLOWER_DOOR, 
*                  GOLDEN_PARAM_ARV_ASYNC_QUEUE_NORMAL_DOOR, 
*                  GOLDEN_PARAM_INDEX_ALWAYS_IN_MEMORY, 
*                  GOLDEN_PARAM_DISK_MIN_REST_SIZE, 
*                  GOLDEN_PARAM_DELAY_OF_AUTO_MERGE_OR_ARRANGE, 
*                  GOLDEN_PARAM_START_OF_AUTO_MERGE_OR_ARRANGE, 
*                  GOLDEN_PARAM_STOP_OF_AUTO_MERGE_OR_ARRANGE, 
*                  GOLDEN_PARAM_START_OF_AUTO_BACKUP, 
*                  GOLDEN_PARAM_STOP_OF_AUTO_BACKUP, 
*                  GOLDEN_PARAM_MAX_LATENCY_OF_SNAPSHOT, 
*                  GOLDEN_PARAM_PAGE_ALLOCATOR_RESERVE_SIZE, 
*      [value]     无符号整型，输入，新的参数值。
* 备注：如果修改了启动参数，将返回 GoE_DATABASE_NEED_RESTART 提示码。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_set_db_info2(
                golden_int32 handle,
                golden_int32 index, 
                golden_uint32 value
                );

/**
* 命名：go_get_logical_drivers
* 功能：获得逻辑盘符
* 参数：
*      [handle]     连接句柄
*      [drivers]    字符数组，输出，
*                   返回逻辑盘符组成的字符串，每个盘符占一个字符。
* 备注：drivers 的内存空间由用户负责维护，长度应不小于 32。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_logical_drivers(
                       golden_int32 handle,
                       char *drivers 
                       );

/**
* 命名：go_open_path
* 功能：打开目录以便遍历其中的文件和子目录。
* 参数：
*      [handle]       连接句柄
*      [dir]          字符串，输入，要打开的目录
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_open_path(
             golden_int32 handle,
             const char *dir 
             );

/**
* 命名：go_read_path
* 功能：读取目录中的文件或子目录
* 参数：
*      [handle]      连接句柄
*      [path]        字符数组，输出，返回的文件、子目录全路径
*      [is_dir]      短整数，输出，返回 1 为目录，0 为文件
*      [atime]       整数，输出，为文件时，返回访问时间 
*      [ctime]       整数，输出，为文件时，返回建立时间 
*      [mtime]       整数，输出，为文件时，返回修改时间 
*      [size]        64 位整数，输出，为文件时，返回文件大小
* 备注：path 的内存空间由用户负责维护，尺寸应不小于 GOLDEN_MAX_PATH。
*      当返回值为 GoE_BATCH_END 时表示目录下所有子目录和文件已经遍历完毕。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_read_path(
             golden_int32 handle,
             char *path ,
             golden_int16 *is_dir,
             golden_int32 *atime,
             golden_int32 *ctime,
             golden_int32 *mtime,
             golden_int64 *size 
             );

/**
* 命名：go_close_path
* 功能：关闭当前遍历的目录
* 参数：
*      [handle]      连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_close_path(
              golden_int32 handle
              );

/**
* 命名：go_mkdir
* 功能：建立目录
* 参数：
*      [handle]       连接句柄
*      [dir]          字符串，输入，新建目录的全路径
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_mkdir(
         golden_int32 handle,
         const char *dir
         );

/**
* 命名：go_get_file_size
* 功能：获得指定服务器端文件的大小
* 参数：
*      [handle]     连接句柄
*      [file]       字符串，输入，文件名
*      [size]       64 位整数，输出，文件大小
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_get_file_size(
                 golden_int32 handle,
                 const char *file,
                 golden_int64 *size
                 );

/**
* 命名：go_read_file
* 功能：读取服务器端指定文件的内容
* 参数：
*      [handle]       连接句柄
*      [file]         字符串，输入，要读取内容的文件名
*      [content]      字符数组，输出，文件内容
*      [pos]          64 位整型，输入，读取文件的起始位置
*      [size]         整型，输入/输出，
*                     输入时表示要读取文件内容的字节大小；
*                     输出时表示实际读取的字节数
* 备注：用户须保证分配给 content 的空间与 size 相符。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
go_read_file(
             golden_int32 handle,
             const char *file, 
             char *content,
             golden_int64 pos,
             golden_int64 *size
             );


/*
* 命名：go_get_max_blob_len
* 功能：取得数据库允许的blob、str类型测点的最大长度
* 参数：
*      [handle]       连接句柄
*      [len]          整形，输出参数，代表数据库允许的blob、str类型测点的最大长度
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_get_max_blob_len(
                    golden_int32 handle,
                    golden_int32 *len
                    );

/*
* 命名：go_format_quality
* 功能：取得质量码对应的定义
* 参数：
*      [handle]       连接句柄
*      [count]        质量码个数，输入参数，
*      [qualities]    质量码，输入参数
*      [definitions]  输出参数，0~255为GOLDEN质量码（参加golden.h文件），256~511为OPC质量码，大于511为用户自定义质量码
*      [lens]         输入/输出参数
*                     输入时表示每个缓冲区大小
*                     输出时表示每个定义长度
* OPC质量码把8位分3部分定义：XX XXXX XX，对应着：品质位域 分状态位域 限定位域
* 品质位域：00（Bad）01（Uncertain）10（N/A）11（Good）
* 分状态位域：不同品质位域对应各自的分状态位域
* 限定位域：00（Not limited）01（Low limited）10（high limited）11（Constant）
* 三个域之间用逗号隔开，输出到definitions参数中，前面有有GOLDEN，OPC或者USER标识，说明标签点类别
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_format_quality(
                  golden_int32 handle,
                  golden_int32 *count,
                  golden_int16 *qualities,
                  golden_byte **definitions,
                  golden_int32 *lens
                  );

/*
* 命名：go_write_named_type_field_by_name
* 功能：按名称填充自定义类型数值中字段的内容
* 参数：
*      [handle]       连接句柄
*      [type_name]    自定义类型的名称，名称长度不能超过GOLDEN_TYPE_NAME_SIZE的长度，输入参数，
*      [field_name]   自定义类型中需要填充的字段的名称，名称长度不能超过GOLDEN_TYPE_NAME_SIZE的长度，输入参数
*      [field_type]   field_name字段的类型，GOLDEN_TYPE所支持的基础类型，输入参数
*      [object]       自定义类型数值的缓冲区,输入/输出参数
*     [object_len]   object缓冲区的长度,输入参数
*     [field]      需要填充的字段数值的缓冲区,输入参数
*     [field_len]    自定义类型中字段数值的缓冲区中数据的长度,输入参数
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_write_named_type_field_by_name(
        golden_int32 handle,
        const char* type_name,
        const char* field_name,
        golden_int32 field_type,
        void* object,
        golden_int16 object_len,
        const void* field,
        golden_int16 field_len
        );

/*
* 命名：go_write_named_type_field_by_pos
* 功能：按位置填充自定义类型数值中字段的内容
* 参数：
*      [handle]       连接句柄
*      [type_name]    自定义类型的名称，名称长度不能超过GOLDEN_TYPE_NAME_SIZE的长度，输入参数，
*      [field_pos]    自定义类型中需要填充的字段的位置，指字段在所有字段中的位置，从0开始，输入参数
*      [field_type]   field_pos位置所在字段的类型，GOLDEN_TYPE所支持的基础类型，输入参数
*      [object]       自定义类型数值的缓冲区,输入/输出参数
*     [object_len]   object缓冲区的长度,输入参数
*     [field]      需要填充的字段数值的缓冲区,输入参数
*     [field_len]    自定义类型中字段数值的缓冲区中数据的长度,输入参数
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_write_named_type_field_by_pos(
      golden_int32 handle,
      const char* type_name,
      golden_int32 field_pos,
      golden_int32 field_type,
      void* object,
      golden_int16 object_len,
      const void* field,
      golden_int16 field_len
      );


/*
* 命名：go_read_named_type_field_by_name
* 功能：按名称提取自定义类型数值中字段的内容
* 参数：
*      [handle]       连接句柄
*      [type_name]    自定义类型的名称，名称长度不能超过GOLDEN_TYPE_NAME_SIZE的长度，输入参数，
*      [field_name]   自定义类型中需要提取的字段的名称，名称长度不能超过GOLDEN_TYPE_NAME_SIZE的长度，输入参数
*      [field_type]   field_name字段的类型，GOLDEN_TYPE所支持的基础类型，输入参数
*      [object]       自定义类型数值的缓冲区,输入参数
*     [object_len]   object缓冲区的长度,输入参数
*     [field]      被读取的字段的数值的缓冲区,输入/输出参数
*     [field_len]    field字段数值缓冲区的长度,输入参数
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_read_named_type_field_by_name(
      golden_int32 handle,
      const char* type_name,
      const char* field_name,
      golden_int32 field_type,
      const void* object,
      golden_int16 object_len,
      void* field,
      golden_int16 field_len
      );


/*
* 命名：go_read_named_type_field_by_pos
* 功能：按位置提取自定义类型数值中字段的内容
* 参数：
*      [handle]       连接句柄
*      [type_name]    自定义类型的名称，名称长度不能超过GOLDEN_TYPE_NAME_SIZE的长度，输入参数，
*      [field_pos]    自定义类型中需要提取的字段的位置，指字段在所有字段中的位置，从0开始，输入参数
*      [field_type]   field_pos位置所在字段的类型，GOLDEN_TYPE所支持的基础类型，输入参数
*      [object]       自定义类型数值的缓冲区,输入参数
*     [object_len]   object缓冲区的长度,输入参数
*     [field]      被读取的字段的数值的缓冲区,输入/输出参数
*     [field_len]    field字段数值缓冲区的长度,输入参数
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_read_named_type_field_by_pos(
        golden_int32 handle,
        const char* type_name,
        golden_int32 field_pos,
        golden_int32 field_type,
        const void* object,
        golden_int16 object_len,
        void* field,
        golden_int16 field_len
        );

/*
* 命名：go_named_type_name_field_check
* 功能：检查自定义类型名称及字段命名是否符合规则；
* 规则：1. 只允许使用26个英文字母,数字0-9，下划线；
*       2. 必须以字母作为首字母；
*       3. 大小写不敏感。
* 参数：
*      [check_name]   需要检查的名称
*      [flag]         标志0--类型名称，其他 -- 字段名称，暂不启用
*/

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
go_named_type_name_field_check(
  const char* check_name,
  golden_byte flag = 0
  );

/*
* 命名：go_judge_connect_status
* 功能：判断连接是否可用；
* 参数：
*      [handle]   连接句柄
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  go_judge_connect_status(golden_int32 handle);

/**
* 命名：go_format_ipaddr
* 功能：将整形IP转换为字符串形式的IP
* 参数：
*      [ip]        无符号整型，输入，整形的IP地址
*      [ip_addr]      字符串，输出，字符串IP地址缓冲区
*      [size]         整型，输入，ip_addr 参数的字节长度
* 备注：用户须保证分配给 ip_addr 的空间与 size 相符
*/
GOLDENAPI
void
GOLDENAPI_CALLRULE
go_format_ipaddr(golden_uint32 ip, char* ip_addr, golden_int32 size);

/************************************ 基本标签点信息接口 ************************************/

/**
* 命名：gob_get_equation_by_file_name
* 功能：根据文件名获取方程式
* 参数：
*      [handle]   连接句柄
*      [file_name] 输入，字符串，方程式路径
*      [equation]  输出，返回的方程式长度最长为GOLDEN_MAX_EQUATION_SIZE-1
*	
*备注：用户调用时为equation分配的空间不得小于GOLDEN_MAX_EQUATION_SIZE
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_get_equation_by_file_name(
												     golden_int32 handle,
												     const char* file_name,
												     char equation[GOLDEN_MAX_EQUATION_SIZE]
												     );

/**
* 命名：gob_get_equation_by_id
* 功能：根ID径获取方程式
* 参数：
*      [handle]   连接句柄
*      [id]				输入，整型，方程式ID
*      [equation] 输出，返回的方程式长度最长为GOLDEN_MAX_EQUATION_SIZE-1
*	
*备注：用户调用时为equation分配的空间不得小于GOLDEN_MAX_EQUATION_SIZE
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_get_equation_by_id(
											 golden_int32 handle,
											 golden_int32 id,
											 char equation[GOLDEN_MAX_EQUATION_SIZE]
											 );


/**
* 命名：gob_append_table
* 功能：添加新表
* 参数：
*      [handle]   连接句柄
*      [field]    GOLDEN_TABLE 结构，输入/输出，表信息。
*                 在输入时，type、name、desc 字段有效；
*                 输出时，id 字段由系统自动分配并返回给用户。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_append_table(
                 golden_int32 handle,
                 GOLDEN_TABLE *field
                 );

/**
* 命名：gob_tables_count
* 功能：取得标签点表总数
* 参数：
*      [handle]   连接句柄
*      [count]    整型，输出，标签点表总数
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_tables_count(
                 golden_int32 handle, 
                 golden_int32 *count
                 );

/**
* 命名：gob_get_tables
* 功能：取得所有标签点表的ID
* 参数：
*      [handle]   连接句柄
*      [ids]      整型数组，输出，标签点表的id
*      [count]    整型，输入/输出，
*                 输入表示 ids 的长度，输出表示标签点表个数
* 备注：用户须保证分配给 ids 的空间与 count 相符
*      如果输入的 count 小于输出的 count，则只返回部分表id
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_tables(
               golden_int32 handle,
               golden_int32 *ids, 
               golden_int32 *count
               );

/**
* 命名：gob_get_table_size_by_id
* 功能：根据表 id 获取表中包含的标签点数量
* 参数：
*      [handle]   连接句柄
*      [id]       整型，输入，表ID
*      [size]     整型，输出，表中标签点数量
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_table_size_by_id(
                         golden_int32 handle,
                         golden_int32 id, 
                         golden_int32 *size
                         );

/**
* 命名：gob_get_table_size_by_name
* 功能：根据表名称获取表中包含的标签点数量
* 参数：
*      [handle]   连接句柄
*      [name]     字符串，输入，表名称
*      [size]     整型，输出，表中标签点数量
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_table_size_by_name(
                           golden_int32 handle,
                           const char *name, 
                           golden_int32 *size
                           );

/**
* 命名：gob_get_table_real_size_by_id
* 功能：根据表 id 获取表中实际包含的标签点数量
* 参数：
*      [handle]   连接句柄
*      [id]       整型，输入，表ID
*      [size]     整型，输出，表中标签点数量
* 注意：通过此API获取标签点数量，然后搜索此表中的标签点得到的数量可能会不一致，这是由于服务内部批量建点采取了异步的方式。
*       一般情况下请使用gob_get_table_size_by_id来获取表中的标签点数量。
*/
GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  gob_get_table_real_size_by_id(
  golden_int32 handle,
  golden_int32 id, 
  golden_int32 *size
  );

/**
* 命名：gob_get_table_property_by_id
* 功能：根据标签点表 id 获取表属性
* 参数：
*      [handle] 连接句柄
*      [field]  GOLDEN_TABLE 结构，输入/输出，标签点表属性，
*               输入时指定 id 字段，输出时返回 type、name、desc 字段。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_table_property_by_id(
                             golden_int32 handle,
                             GOLDEN_TABLE *field
                             );

/**
* 命名：gob_get_table_property_by_name
* 功能：根据表名获取标签点表属性
* 参数：
*      [handle] 连接句柄
*      [field]  GOLDEN_TABLE 结构，输入/输出，标签点表属性
*               输入时指定 name 字段，输出时返回 id、type、desc 字段。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_table_property_by_name(
                               golden_int32 handle,
                               GOLDEN_TABLE *field
                               );

/**
* 命名：gob_insert_point
* 功能：使用完整的属性集来创建单个标签点
* 参数：
*      [handle] 连接句柄
*      [base] GOLDEN_POINT 结构，输入/输出，
*      输入除 id, createdate, creator, changedate, changer 字段外的其它字段，输出 id 字段。
*      [scan] GOLDEN_SCAN_POINT 结构，输入，采集标签点扩展属性集。
*      [calc] GOLDEN_CALC_POINT 结构，输入，计算标签点扩展属性集。
* 备注：如果新建的标签点没有对应的扩展属性集，可置为空指针。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_insert_point(
                 golden_int32 handle, 
                 GOLDEN_POINT *base, 
                 GOLDEN_SCAN_POINT *scan, 
                 GOLDEN_CALC_POINT *calc
                 );


/**
* 命名：gob_insert_max_point
* 功能：使用最大长度的完整属性集来创建单个标签点
* 参数：
*      [handle] 连接句柄
*      [base] GOLDEN_POINT 结构，输入/输出，
*      输入除 id, createdate, creator, changedate, changer 字段外的其它字段，输出 id 字段。
*      [scan] GOLDEN_SCAN_POINT 结构，输入，采集标签点扩展属性集。
*      [calc] GOLDEN_MAX_CALC_POINT 结构，输入，计算标签点扩展属性集。
* 备注：如果新建的标签点没有对应的扩展属性集，可置为空指针。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_insert_max_point(
										 golden_int32 handle,
										 GOLDEN_POINT *base,
										 GOLDEN_SCAN_POINT *scan,
										 GOLDEN_MAX_CALC_POINT *calc
										 );

/**
* 命名：gob_insert_base_point
* 功能  使用最小的属性集来创建单个标签点
* 参数：
*      [handle]     连接句柄
*      [tag]        字符串，输入，标签点名称
*      [type]       整型，输入，标签点数据类型，取值 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、
*                   GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64、
*                   GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64、GOLDEN_COOR、GOLDEN_STRING、GOLDEN_BLOB 之一。
*      [table_id]   整型，输入，标签点所属表 id
*      [use_ms]     短整型，输入，标签点时间戳精度，0 为秒；1 为毫秒。
*      [point_id]   整型，输出，标签点 id
* 备注：标签点的其余属性将取默认值。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_insert_base_point(
                      golden_int32 handle, 
                      const char *tag, 
                      golden_int32 type, 
                      golden_int32 table_id, 
                      golden_int16 use_ms, 
                      golden_int32 *point_id
                      );

/**
* 命名：gob_insert_named_type_point
* 功能：使用完整的属性集来创建单个自定义数据类型标签点
* 参数：
*      [handle] 连接句柄
*      [base] GOLDEN_POINT 结构，输入/输出，
*      输入除 id, createdate, creator, changedate, changer 字段外的其它字段，输出 id 字段。
*      [scan] GOLDEN_SCAN_POINT 结构，输入，采集标签点扩展属性集。
*      [name] 字符串，输入，自定义数据类型的名字。
* 备注：如果新建的标签点没有对应的扩展属性集，可置为空指针。
*/
GOLDENAPI
  golden_error 
  GOLDENAPI_CALLRULE 
  gob_insert_named_type_point(
  golden_int32 handle, 
  GOLDEN_POINT *base, 
  GOLDEN_SCAN_POINT *scan, 
  const char* name
  );

/**
* 命名：gob_remove_point_by_id
* 功能：根据 id 删除单个标签点
* 参数：
*      [handle] 连接句柄
*      [id]     整型，输入，标签点标识
* 备注：通过本接口删除的标签点为可回收标签点，
*        可以通过 gob_recover_point 接口恢复。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_remove_point_by_id(
                       golden_int32 handle,
                       golden_int32 id
                       );

/**
* 命名：gob_remove_point_by_name
* 功能：根据标签点全名删除单个标签点
* 参数：[handle]        连接句柄
*        [table_dot_tag]  字符串，输入，标签点全名称："表名.标签点名"
* 备注：通过本接口删除的标签点为可回收标签点，
*        可以通过 gob_recover_point 接口恢复。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_remove_point_by_name(
                         golden_int32 handle,
                         const char *table_dot_tag
                         );

/**
* 命名：gob_get_points_property
* 功能：批量获取标签点属性
* 参数：
*        [handle] 连接句柄
*        [count]  整数，输入，表示标签点个数。
*        [base]   GOLDEN_POINT 结构数组，输入/输出，标签点基本属性列表，
*                 输入时，id 字段指定需要得到属性的标签点，输出时，其它字段返回标签点属性值。
*        [scan]   GOLDEN_SCAN_POINT 结构数组，输出，采集标签点扩展属性列表
*        [calc]   GOLDEN_CALC_POINT 结构数组，输出，计算标签点扩展属性列表
*        [errors] 无符号整型数组，输出，获取标签属性的返回值列表，参考golden_error.h
* 备注：用户须保证分配给 base、scan、calc、errors 的空间与 count 相符，
*        扩展属性集 scan、calc 可为空指针，此时将不返回对应的扩展属性集。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_get_points_property(
                        golden_int32 handle,
                        golden_int32 count,
                        GOLDEN_POINT *base,
                        GOLDEN_SCAN_POINT *scan,
                        GOLDEN_CALC_POINT *calc,
                        golden_error *errors
                        );


/**
* 命名：gob_get_max_points_property
* 功能：按最大长度批量获取标签点属性
* 参数：
*        [handle] 连接句柄
*        [count]  整数，输入，表示标签点个数。
*        [base]   GOLDEN_POINT 结构数组，输入/输出，标签点基本属性列表，
*                 输入时，id 字段指定需要得到属性的标签点，输出时，其它字段返回标签点属性值。
*        [scan]   GOLDEN_SCAN_POINT 结构数组，输出，采集标签点扩展属性列表
*        [calc]   GOLDEN_MAX_CALC_POINT 结构数组，输出，计算标签点扩展属性列表
*        [errors] 无符号整型数组，输出，获取标签属性的返回值列表，参考golden_error.h
* 备注：用户须保证分配给 base、scan、calc、errors 的空间与 count 相符，
*        扩展属性集 scan、calc 可为空指针，此时将不返回对应的扩展属性集。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_get_max_points_property(
														golden_int32 handle,
														golden_int32 count,
														GOLDEN_POINT *base,
														GOLDEN_SCAN_POINT *scan,
														GOLDEN_MAX_CALC_POINT *calc,
														golden_error *errors
														);



/**
* 命名：gob_search
* 功能：搜索符合条件的标签点，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [tablemask]     字符串，输入，标签点表名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [mode]          整型，GOLDEN_SORT_BY_TABLE、GOLDEN_SORT_BY_TAG、GOLDEN_SORT_BY_ID 之一，
*                        搜索结果的排序模式，输入，缺省值为GOLDEN_SORT_BY_TABLE
*        [ids]           整型数组，输出，返回搜索到的标签点标识列表
*        [count]         整型，输入/输出，输入时表示 ids 的长度，输出时表示搜索到的标签点个数
* 备注：用户须保证分配给 ids 的空间与 count 相符，各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、tablemask 为空指针，则表示使用缺省设置"*",
*        多个搜索条件可以通过空格分隔，比如"demo_*1 demo_*2"，会将满足demo_*1或者demo_*2条件的标签点搜索出来。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_search(
           golden_int32 handle,
           const char *tagmask,
           const char *tablemask,
           const char *source,
           const char *unit,
           const char *desc,
           const char *instrument,
           golden_int32 mode,
           golden_int32 *ids, 
           golden_int32 *count
          );

/**
* 命名：gob_search_in_batches
* 功能：分批继续搜索符合条件的标签点，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [start]         整型，输入，搜索起始位置（指满足条件的起始数）。
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [tablemask]     字符串，输入，标签点表名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [mode]          整型，GOLDEN_SORT_BY_TABLE、GOLDEN_SORT_BY_TAG、GOLDEN_SORT_BY_ID 之一，
*                        搜索结果的排序模式，输入，缺省值为GOLDEN_SORT_BY_TABLE
*        [ids]           整型数组，输出，返回搜索到的标签点标识列表
*        [count]         整型，输入/输出，输入时表示 ids 的长度，输出时表示搜索到的标签点个数
* 备注：用户须保证分配给 ids 的空间与 count 相符，各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、tablemask 为空指针，则表示使用缺省设置"*"。
*        当搜索到的标签点数比提供的要小时，表示这是最后一批符合条件的标签点 (即全部搜索完毕),
*        多个搜索条件可以通过空格分隔，比如"demo_*1 demo_*2"，会将满足demo_*1或者demo_*2条件的标签点搜索出来。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_search_in_batches(
                      golden_int32 handle,
                      golden_int32 start, 
                      const char *tagmask,
                      const char *tablemask,
                      const char *source,
                      const char *unit,
                      const char *desc,
                      const char *instrument,
                      golden_int32 mode,
                      golden_int32 *ids, 
                      golden_int32 *count
                      );


/**
* 命名：gob_search_ex
* 功能：搜索符合条件的标签点，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [tablemask]     字符串，输入，标签点表名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [typemask]      字符串，输入参数，标签点类型名称。缺省设置为空，长度不得超过 GOLDEN_TYPE_NAME_SIZE,
*                        内置的普通数据类型可以使用 bool、uint8、datetime等字符串表示，不区分大小写，支持模糊搜索。
*        [classofmask]   整型，输入参数，标签点的类别，缺省设置为-1，表示可以是任意类型的标签点，
*                        当使用标签点类型作为搜索条件时，必须是GOLDEN_CLASS枚举中的一项或者多项的组合。
*        [timeunitmask]  整型，输入参数，标签点的时间戳精度，缺省设置为-1，表示可以是任意时间戳精度，
*                        当使用此时间戳精度作为搜索条件时，timeunitmask的值可以为0或1，0表示时间戳精度为秒，1表示毫秒
*        [othertypemask] 整型，输入参数，使用其他标签点属性作为搜索条件，缺省设置为0，表示不作为搜索条件，
*                        当使用此参数作为搜索条件时，othertypemaskvalue作为对应的搜索值，
*                        此参数的取值可以参考golden.h文件中的GOLDEN_SEARCH_MASK。
*        [othertypemaskvalue]
*                        字符串，输入参数，当使用其他标签点属性作为搜索条件时，此参数作为对应的搜索值，缺省设置为0，表示不作为搜索条件，
*                        如果othertypemask的值为0，或者GOLDEN_SEARCH_NULL，则此参数被忽略,
*                        当othertypemask对应的标签点属性为数值类型时，此搜索值只支持相等判断，
*                        当othertypemask对应的标签点属性为字符串类型时，此搜索值支持模糊搜索。
*        [mode]          整型，GOLDEN_SORT_BY_TABLE、GOLDEN_SORT_BY_TAG、GOLDEN_SORT_BY_ID 之一，
*                        搜索结果的排序模式，输入，缺省值为GOLDEN_SORT_BY_TABLE
*        [ids]           整型数组，输出，返回搜索到的标签点标识列表
*        [count]         整型，输入/输出，输入时表示 ids 的长度，输出时表示搜索到的标签点个数
* 备注：用户须保证分配给 ids 的空间与 count 相符，各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、tablemask 为空指针，则表示使用缺省设置"*",
*        多个搜索条件可以通过空格分隔，比如"demo_*1 demo_*2"，会将满足demo_*1或者demo_*2条件的标签点搜索出来。
*/
GOLDENAPI
  golden_error  
  GOLDENAPI_CALLRULE 
  gob_search_ex(
  golden_int32 handle,
  const char *tagmask,
  const char *tablemask,
  const char *source,
  const char *unit,
  const char *desc,
  const char *instrument,
  const char *typemask,
  golden_int32 classofmask,
  golden_int32 timeunitmask,
  golden_int32 othertypemask,
  const char *othertypemaskvalue,
  golden_int32 mode,
  golden_int32 *ids, 
  golden_int32 *count
  );


/**
* 命名：gob_search_points_count
* 功能：搜索符合条件的标签点，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [tablemask]     字符串，输入，标签点表名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [typemask]      字符串，输入参数，标签点类型名称。缺省设置为空，长度不得超过 GOLDEN_TYPE_NAME_SIZE,
*                        内置的普通数据类型可以使用 bool、uint8、datetime等字符串表示，不区分大小写，支持模糊搜索。
*        [classofmask]   整型，输入参数，标签点的类别，缺省设置为-1，表示可以是任意类型的标签点，
*                        当使用标签点类型作为搜索条件时，必须是GOLDEN_CLASS枚举中的一项或者多项的组合。
*        [timeunitmask]  整型，输入参数，标签点的时间戳精度，缺省设置为-1，表示可以是任意时间戳精度，
*                        当使用此时间戳精度作为搜索条件时，timeunitmask的值可以为0或1，0表示时间戳精度为秒，1表示毫秒
*        [othertypemask] 整型，输入参数，使用其他标签点属性作为搜索条件，缺省设置为0，表示不作为搜索条件，
*                        当使用此参数作为搜索条件时，othertypemaskvalue作为对应的搜索值，
*                        此参数的取值可以参考golden.h文件中的GOLDEN_SEARCH_MASK。
*        [othertypemaskvalue]
*                        字符串，输入参数，当使用其他标签点属性作为搜索条件时，此参数作为对应的搜索值，缺省设置为0，表示不作为搜索条件，
*                        如果othertypemask的值为0，或者GOLDEN_SEARCH_NULL，则此参数被忽略,
*                        当othertypemask对应的标签点属性为数值类型时，此搜索值只支持相等判断，
*                        当othertypemask对应的标签点属性为字符串类型时，此搜索值支持模糊搜索。
*        [count]         整型，输出，表示搜索到的标签点个数
* 备注： 各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、tablemask 为空指针，则表示使用缺省设置"*",
*        多个搜索条件可以通过空格分隔，比如"demo_*1 demo_*2"，会将满足demo_*1或者demo_*2条件的标签点搜索出来。
*/
GOLDENAPI
  golden_error  
  GOLDENAPI_CALLRULE 
  gob_search_points_count(
  golden_int32 handle,
  const char *tagmask,
  const char *tablemask,
  const char *source,
  const char *unit,
  const char *desc,
  const char *instrument,
  const char *typemask,
  golden_int32 classofmask,
  golden_int32 timeunitmask,
  golden_int32 othertypemask,
  const char *othertypemaskvalue,
  golden_int32 *count
  );

//////////////////////////////////////////////////////////////////////////
/**
* 命名：gob_get_table_field_count
* 功能：获取表字段数量
* 参数：
*        [handle]        连接句柄
*        [table_id]       GOLDEN_TABLE_NAME_ID 类型，输入，支持枚举组合。
*        [field_count]         整型，输出，表示获取到的字段个数
* 备注： table_id 等于GOLDEN_TABLE_ID_BASE，显示基本属性字段信息；等于GOLDEN_TABLE_ID_SCAN，显示采集属性字段信息；
等于GOLDEN_TABLE_ID_CALC，显示计算属性字段信息；等于 GOLDEN_TABLE_ID_BASE|GOLDEN_TABLE_ID_SCAN，同时显示基本属性 + 采集属性信息；
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_get_table_field_count(golden_int32 handle, int class_of, golden_int32* field_count);
/**
* 命名：gob_get_table_fields
* 功能：获取表字段结构
* 参数：
*        [handle]        连接句柄
*        [table_id]       GOLDEN_TABLE_NAME_ID 类型，输入，支持枚举组合。
*        [fields]       GOLDEN_TAG_FIELD数组，输出，获取的字段结体。
*        [field_count]         整型，输入/输出，输入时表示fields数组数量，输出，表示获取到的字段个数
* 备注：  table_id 等于GOLDEN_TABLE_ID_BASE，显示基本属性字段信息；等于GOLDEN_TABLE_ID_SCAN，显示采集属性字段信息；
等于GOLDEN_TABLE_ID_CALC，显示计算属性字段信息；等于 GOLDEN_TABLE_ID_BASE|GOLDEN_TABLE_ID_SCAN，同时显示基本属性 + 采集属性信息；
fields数量输入时必须等于field_count。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_get_table_fields(golden_int32 handle, int class_of, GOLDEN_TAG_FIELD* fields, golden_int32* field_count);
/**
* 命名：gob_get_table_fields
* 功能：通过查询条件，统计符合的数量
* 参数：
*        [handle]        连接句柄
*        [json_str]       字符串，输入参数，json查询条件字符串。
*        [json_str_size]     整型，输入参数，json字符串长度。
*        [id_count]         整型，输出，表示获取到的标签点个数
* 备注： json_str：查询条件，参考mongodb查询条件。
"$lt"、"$lte"、"$gt"、"$gte"、"$ne"、"$in"、"$nin"、"$like"、"$nlike"、"$and"、"$or"分别对应<、<=、>、>=、不等于、包含于、不包含于、模糊包含、模糊不包含、条件与、条件或；
{"id" : 20} 表示id等于20；{"id" : 20 , "tag" : "abc"}表示id=20 && tag=abc；{"id" : {"$gte" : 18, "$lte" : 30}} 表示id>=18 && id<=30；
{"id": {"$ne": 12}}表示id != 12；{"id" : {"$in" : [12,13,14,15]}} 表示id 在12,13,14,15范围内；
{"tag" : {"$like" : "A%"}}表示匹配以A开头的数据；{"tag" : {"$like": "%A%"}}表示搜索tag中间包含A的数据；
{"$or" : [{"id" : 12}, {"tag" : {"$like" : "%a"}}]}表示id=12 || tag like "%a"；
{"$and" : ["id" : {"$gte" : 18, "$lte" : 30}, "tag", {"$like" : "%a"}]}表示取 id>=18 && id<=30 && tag like "%a"
{"$or" : [{"$and" : ["id" : {"$gte" : 18, "$lte" : 30}, "tag" : {"$like" : "%abc%"}]}, {"$and" : ["id" : {"$gte" : 80, "$lte" : 100}, "tag" : {"$nlike" : "abc%"}]}]}
表示取 ((id>=18 && id <= 30 && tag like "%abc%") || (id>=80 && id <= 100 && tag not like "abc%"))
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_search_point_id_count(golden_int32 handle, const char* json_str, const size_t& json_str_size, golden_int32* id_count);
/**
* 命名：gob_search_point_ids
* 功能：查询点信息数据，获取点数组
* 参数：
*        [handle]        连接句柄
*        [json_str]       字符串，输入参数，json查询条件字符串。
*        [json_str_size]     整型，输入参数，json字符串长度。
*        [ids]         整型数组，输出，返回搜索到的标签点标识列表
*        [id_count]         整型，输入，输出，输入时表示 id_array 的长度，输出时表示搜索到的标签点个数
* 备注： json_str：查询条件，参考mongodb查询条件。
"$lt"、"$lte"、"$gt"、"$gte"、"$ne"、"$in"、"$nin"、"$like"、"$nlike"、"$and"、"$or"分别对应<、<=、>、>=、不等于、包含于、不包含于、模糊包含、模糊不包含、条件与、条件或；
{"id" : 20} 表示id等于20；{"id" : 20 , "tag" : "abc"}表示id=20 && tag=abc；{"id" : {"$gte" : 18, "$lte" : 30}} 表示id>=18 && id<=30；
{"id": {"$ne": 12}}表示id != 12；{"id" : {"$in" : [12,13,14,15]}} 表示id 在12,13,14,15范围内；
{"tag" : {"$like" : "A%"}}表示匹配以A开头的数据；{"tag" : {"$like": "%A%"}}表示搜索tag中间包含A的数据；
{"$or" : [{"id" : 12}, {"tag" : {"$like" : "%a"}}]}表示id=12 || tag like "%a"；
{"$and" : ["id" : {"$gte" : 18, "$lte" : 30}, "tag", {"$like" : "%a"}]}表示取 id>=18 && id<=30 && tag like "%a"
{"$or" : [{"$and" : ["id" : {"$gte" : 18, "$lte" : 30}, "tag" : {"$like" : "%abc%"}]}, {"$and" : ["id" : {"$gte" : 80, "$lte" : 100}, "tag" : {"$nlike" : "abc%"}]}]}
表示取 ((id>=18 && id <= 30 && tag like "%abc%") || (id>=80 && id <= 100 && tag not like "abc%"))
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_search_point_ids(golden_int32 handle, const char* json_str, const size_t& json_str_size, golden_int32* ids, golden_int32* id_count);
/**
* 命名：gob_append_point
* 功能：批量添加点信息
* 参数：
*        [handle]        连接句柄
*        [json_str]       字符串，输入参数，json字符串；
*        [json_str_size]     整型，输入参数，json字符串长度。
*        [id]         整型，输出，返回添加成功的标签点编号
* 备注： json_str：标签点信息。示例如下：
{"tag":"456","type":3,"table":4,"desc":"456",...}
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_append_point(golden_int32 handle, const char* json_str, const size_t& json_str_size, golden_int32* id);
/**
* 命名：gob_update_points
* 功能：批量更新点信息
* 参数：
*        [handle]        连接句柄
*        [id]              标签点编号  
*        [json_str]       字符串，输入参数，json字符串；
*        [json_str_size]     整型，输入参数，json字符串长度。
* 备注： json_str：标签点信息。示例如下：
{"id":1,"tag":"tag1","desc":"tag1 desc",...}
依id更新内容， id、table、type、millisecond 字段不能修改；
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_update_point(golden_int32 handle, golden_int32 id, const char* json_str, const size_t& json_str_size);
/**
* 命名：gob_remove_points
* 功能：批量删除点信息
* 参数：
*        [handle]        连接句柄
*        [auto_purge]       整型，输入参数，是否自动物理删除，0为不作物理删除，非0则物理删；
*        [ids]     整型数组，输入参数，删除标签点数组。
*        [id_count]         整型，输入/输出，输入时表示数组数量，输出时表示删除成功数量
*        [err_codes]         整型数组，输出，返回添加是否成功
* 备注：ids，err_codes初始化长度必须等于id_count数量，
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_remove_points(golden_int32 handle, int auto_purge, const golden_int32* ids, golden_int32* id_count, golden_error* err_codes);
/**
* 命名：gob_search_point_props
* 功能：根据条件获取指定字段内容
* 参数：
*        [handle]        连接句柄
*        [json_str]       字符串，输入参数，json字符串；
*        [json_str_size]     整型，输入参数，json字符串长度；
*        [tag_count]         整数，输出参数，返回获取到标签点数量
*        [prop_size]         整型，输出，表示返回json字符串长度；
*        [json_props]       字符串数组，输入时 *json_props需要为0，输出时json_props为json字符串，
*                                 必须使用go_release_pointer释放，否则会出现内存泄露
* 备注： json_str：查询条件及选择返回字段；
* fields 使用*表示返回所有字段，使用base表示返回base基本属性字段，scan表示返回scan采集属性字段，calc表示返回calc计算属性字段。
* condition 使用方法参见：gob_search_point_ids中的条件描述。
json_str格式如下：
{"fields":["id","tag","type"],
"condition":{}
}，
json_props格式如下：
[
  {"id":23,"tag":"tag1","type":1...},
  {"id":24,"tag":"tag2","type":3...},
  ...
]
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_search_point_props(golden_int32 handle, const char* json_str, const size_t& json_str_size, int* tag_count, size_t* prop_size, char** json_props);

/**
* 命名：gob_remove_table_by_id
* 功能：根据表 id 删除表及表中标签点
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，表 id 
* 备注：删除的表不可恢复，删除的标签点可以通过 gob_recover_point 接口恢复。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_remove_table_by_id(
                       golden_int32 handle, 
                       golden_int32 id
                       );

/**
* 命名：gob_remove_table_by_name
* 功能：根据表名删除表及表中标签点
* 参数：
*        [handle]        连接句柄
*        [name]          字符串，输入，表名称
* 备注：删除的表不可恢复，删除的标签点可以通过 gob_recover_point 接口恢复。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_remove_table_by_name(
                         golden_int32 handle, 
                         const char *name
                         );

/**
* 命名：gob_update_point_property
* 功能：更新单个标签点属性
* 参数：
*        [handle]        连接句柄
*        [base] GOLDEN_POINT 结构，输入，基本标签点属性集。
*        [scan] GOLDEN_SCAN_POINT 结构，输入，采集标签点扩展属性集。
*        [calc] GOLDEN_CALC_POINT 结构，输入，计算标签点扩展属性集。
* 备注：标签点由 base 参数的 id 字段指定，其中 id、table、type、millisecond 字段不能修改，
*      changedate、changer、createdate、creator 字段由系统维护，其余字段均可修改，
*      包括 classof 字段。输入参数中 scan、calc 可为空指针，对应的扩展属性将保持不变。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_update_point_property(
                          golden_int32 handle,
                          const GOLDEN_POINT *base,
                          const GOLDEN_SCAN_POINT *scan,
                          const GOLDEN_CALC_POINT *calc
                          );

///**
//* 命名：gob_update_min_point_property
//* 功能：按最小长度更新单个标签点属性
//* 参数：
//*        [handle]        连接句柄
//*        [base] GOLDEN_POINT 结构，输入，基本标签点属性集。
//*        [scan] GOLDEN_SCAN_POINT 结构，输入，采集标签点扩展属性集。
//*        [calc] GOLDEN_MAX_CALC_POINT 结构，输入，计算标签点扩展属性集。
//* 备注：标签点由 base 参数的 id 字段指定，其中 id、table、type、millisecond 字段不能修改，
//*      changedate、changer、createdate、creator 字段由系统维护，其余字段均可修改，
//*      包括 classof 字段。输入参数中 scan、calc 可为空指针，对应的扩展属性将保持不变。
//*/
//GOLDENAPI
//golden_error
//GOLDENAPI_CALLRULE
//gob_update_min_point_property(
//															golden_int32 handle,
//															const GOLDEN_POINT *base,
//															const GOLDEN_SCAN_POINT *scan,
//															const GOLDEN_MAX_CALC_POINT *calc
//															);

/**
* 命名：gob_update_max_point_property
* 功能：按最大长度更新单个标签点属性
* 参数：
*        [handle]        连接句柄
*        [base] GOLDEN_POINT 结构，输入，基本标签点属性集。
*        [scan] GOLDEN_SCAN_POINT 结构，输入，采集标签点扩展属性集。
*        [calc] GOLDEN_MAX_CALC_POINT 结构，输入，计算标签点扩展属性集。
* 备注：标签点由 base 参数的 id 字段指定，其中 id、table、type、millisecond 字段不能修改，
*      changedate、changer、createdate、creator 字段由系统维护，其余字段均可修改，
*      包括 classof 字段。输入参数中 scan、calc 可为空指针，对应的扩展属性将保持不变。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_update_max_point_property(
															golden_int32 handle,
															const GOLDEN_POINT *base,
															const GOLDEN_SCAN_POINT *scan,
															const GOLDEN_MAX_CALC_POINT *calc
															);


/**
* 命名：gob_find_points
* 功能：根据 "表名.标签点名" 格式批量获取标签点标识
* 参数：
*        [handle]           连接句柄
*        [count]            整数，输入/输出，输入时表示标签点个数
*                           (即table_dot_tags、ids、types、classof、use_ms 的长度)，
*                           输出时表示找到的标签点个数
*        [table_dot_tags]   字符串指针数组，输入，"表名.标签点名" 列表
*        [ids]              整型数组，输出，标签点标识列表, 返回 0 表示未找到
*        [types]            整型数组，输出，标签点数据类型
*        [classof]          整型数组，输出，标签点类别
*        [use_ms]           短整型数组，输出，时间戳精度，
*                           返回 1 表示时间戳精度为毫秒， 为 0 表示为秒。
* 备注：用户须保证分配给 table_dot_tags、ids、types、classof、use_ms 的空间与count相符，
*        其中 types、classof、use_ms 可为空指针，对应的字段将不再返回。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_find_points(
                golden_int32 handle, 
                golden_int32 *count, 
                const char* const* table_dot_tags,
                golden_int32 *ids,
                golden_int32 *types,
                golden_int32 *classof, 
                golden_int16 *use_ms
                );

/**
* 命名：gob_sort_points
* 功能：根据标签属性字段对标签点标识进行排序
* 参数：
*        [handle]           连接句柄
*        [count]            整数，输入，表示标签点个数, 即 ids 的长度
*        [ids]              整型数组，输入，标签点标识列表
*        [index]            整型，输入，属性字段枚举，参见 GOLDEN_TAG_FIELD_INDEX，
*                           将根据该字段对 ID 进行排序。
*        [flag]             整型，输入，标志位组合，参见 GOLDEN_TAG_SORT_FLAG 枚举，其中
*                           GOLDEN_SORT_FLAG_DESCEND             表示降序排序，不设置表示升序排列；
*                           GOLDEN_SORT_FLAG_CASE_SENSITIVE      表示进行字符串类型字段比较时大小写敏感，不设置表示不区分大小写；
*                           GOLDEN_SORT_FLAG_RECYCLED            表示对可回收标签进行排序，不设置表示对正常标签排序，
*                           不同的标志位可通过"或"运算连接在一起，
*                           当对可回收标签排序时，以下字段索引不可使用：
*                               GOLDEN_TAG_INDEX_TIMESTAMP
*                               GOLDEN_TAG_INDEX_VALUE
*                               GOLDEN_TAG_INDEX_QUALITY
* 备注：用户须保证分配给 ids 的空间与 count 相符, 如果 ID 指定的标签并不存在，
*        或标签不具备要求排序的字段 (如对非计算点进行方程式排序)，它们将被放置在数组的尾部。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_sort_points(
                golden_int32 handle, 
                golden_int32 count, 
                golden_int32 *ids,
                golden_int32 index, 
                golden_int32 flag
                );

/**
* 命名：gob_update_table_name
* 功能：根据表 ID 更新表名称。
* 参数：
*        [handle]    连接句柄
*        [tab_id]    整型，输入，要修改表的标识
*        [name]      字符串，输入，新的标签点表名称。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_update_table_name(
                      golden_int32 handle, 
                      golden_int32 tab_id, 
                      const char *name
                      );

/**
* 命名：gob_update_table_desc_by_id
* 功能：根据表 ID 更新表描述。
* 参数：
*        [handle]    连接句柄
*        [tab_id]    整型，输入，要修改表的标识
*        [desc]      字符串，输入，新的表描述。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_update_table_desc_by_id(
                            golden_int32 handle, 
                            golden_int32 tab_id, 
                            const char *desc
                            );

/**
* 命名：gob_update_table_desc_by_name
* 功能：根据表名称更新表描述。
* 参数：
*        [handle]    连接句柄
*        [name]      字符串，输入，要修改表的名称。
*        [desc]      字符串，输入，新的表描述，清空描述时传入长度为空的字符串。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_update_table_desc_by_name(
                              golden_int32 handle, 
                              const char *name, 
                              const char *desc
                              );

/**
* 命名：gob_recover_point
* 功能：恢复已删除标签点
* 参数：
*        [handle]    连接句柄
*        [table_id]  整型，输入，要将标签点恢复到的表标识
*        [point_id]  整型，输入，待恢复的标签点标识
* 备注: 本接口只对可回收标签点(通过接口gob_remove_point_by_id/gob_remove_point_by_tag)有效，
*        对正常的标签点没有作用。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_recover_point(
                  golden_int32 handle,
                  golden_int32 table_id,
                  golden_int32 point_id
                  );

/**
* 命名：gob_purge_point
* 功能：清除标签点
* 参数：
*        [handle]    连接句柄
*        [id]        整数，输入，要清除的标签点标识
* 备注: 本接口仅对可回收标签点(通过接口gob_remove_point_by_id/gob_remove_point_by_name)有效，
*      对正常的标签点没有作用。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_purge_point(
                golden_int32 handle,
                golden_int32 id
                );


/**
* 命名：gob_get_recycled_points_count
* 功能：获取可回收标签点数量
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输出，可回收标签点的数量
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_recycled_points_count(
                              golden_int32 handle,
                              golden_int32 *count
                              );

/**
* 命名：gob_get_recycled_points
* 功能：获取可回收标签点 id 列表
* 参数：
*        [handle]    连接句柄
*        [ids]       整型数组，输出，可回收标签点 id
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids 的长度，
*                    输出时表示成功获取标签点的个数。
* 备注：用户须保证 ids 的长度与 count 一致
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_recycled_points(
                        golden_int32 handle, 
                        golden_int32 *ids,
                        golden_int32 *count
                        );

/**
* 命名：gob_search_recycle_points_count
* 功能：回收站中搜索符合条件的标签点的个数，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [fullmask]      字符串，输入，标签点全名掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE+GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE，支持多个搜索条件，以空格分隔。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [count]         整型，输出，表示搜索到的标签点个数
* 备注： 各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、tablemask 为空指针，则表示使用缺省设置"*",
*        多个搜索条件可以通过空格分隔，比如"demo_*1 demo_*2"，会将满足demo_*1或者demo_*2条件的标签点搜索出来。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_search_recycle_points_count(
golden_int32 handle,
const char *fullmask,
const char *tagmask,
const char *source,
const char *unit,
const char *desc,
const char *instrument,
golden_int32 *count
);
/**
* 命名：gob_search_recycled_points
* 功能：搜索符合条件的可回收标签点，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE。
*        [tablemask]     字符串，输入，标签点表名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [mode]          整型，GOLDEN_SORT_BY_TABLE、GOLDEN_SORT_BY_TAG、GOLDEN_SORT_BY_ID 之一，
*                        搜索结果的排序模式，输入，缺省值为GOLDEN_SORT_BY_TABLE
*        [ids]           整型数组，输出，返回搜索到的标签点标识列表
*        [count]         整型，输入/输出，输入时表示 ids 的长度，输出时表示搜索到的标签点个数
* 备注：用户须保证分配给 ids 的空间与 count 相符，各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、fullmask 为空指针，则表示使用缺省设置"*"
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_search_recycled_points(
                           golden_int32 handle,
                           const char *tagmask,
                           const char *fullmask,
                           const char *source,
                           const char *unit,
                           const char *desc,
                           const char *instrument,
                           golden_int32 mode,
                           golden_int32 *ids, 
                           golden_int32 *count
                           );

/**
* 命名：gob_search_recycled_points_in_batches
* 功能：分批搜索符合条件的可回收标签点，使用标签点名时支持通配符
* 参数：
*        [handle]        连接句柄
*        [start]         整型，输入，搜索的起始位置（指满足条件的起始数）。
*        [tagmask]       字符串，输入，标签点名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE。
*        [fullmask]      字符串，输入，表名.标签点全名称掩码，支持"*"和"?"通配符，缺省设置为"*"，长度不得超过 GOLDEN_TAG_SIZE*2。
*        [source]        字符串，输入，数据源集合，字符串中的每个字符均表示一个数据源，
*                        空字符串表示不用数据源作搜索条件，缺省设置为空，长度不得超过 GOLDEN_DESC_SIZE。
*        [unit]          字符串，输入，标签点工程单位的子集，工程单位中包含该参数的标签点均满足条件，
*                        空字符串表示不用工程单位作搜索条件，缺省设置为空，长度不得超过 GOLDEN_UNIT_SIZE。
*        [desc]          字符串，输入，标签点描述的子集，描述中包含该参数的标签点均满足条件，
*                        空字符串表示不用描述作搜索条件，缺省设置为空，长度不得超过 GOLDEN_SOURCE_SIZE。
*        [instrument]    字符串，输入参数，标签点设备名称。缺省设置为空，长度不得超过 GOLDEN_INSTRUMENT_SIZE。
*        [mode]          整型，GOLDEN_SORT_BY_TABLE、GOLDEN_SORT_BY_TAG、GOLDEN_SORT_BY_ID 之一，
*                        搜索结果的排序模式，输入，缺省值为GOLDEN_SORT_BY_TABLE
*        [ids]           整型数组，输出，返回搜索到的标签点标识列表
*        [count]         整型，输入/输出，输入时表示 ids 的长度，输出时表示搜索到的标签点个数
* 备注：用户须保证分配给 ids 的空间与 count 相符，各参数中包含的搜索条件之间的关系为"与"的关系，
*        用包含通配符的标签点名称作搜索条件时，如果第一个字符不是通配符(如"ai67*")，会得到最快的搜索速度。
*        如果 tagmask、fullmask 为空指针，则表示使用缺省设置"*"
*        当搜索到的标签点数比提供的要小时，表示这是最后一批符合条件的标签点 (即全部搜索完毕)。
*/
GOLDENAPI
golden_error  
GOLDENAPI_CALLRULE 
gob_search_recycled_points_in_batches(
                                      golden_int32 handle,
                                      golden_int32 start, 
                                      const char *tagmask,
                                      const char *fullmask,
                                      const char *source,
                                      const char *unit,
                                      const char *desc,
                                      const char *instrument,
                                      golden_int32 mode,
                                      golden_int32 *ids, 
                                      golden_int32 *count
                                      );

/**
* 命名：gob_get_recycled_point_property
* 功能：获取可回收标签点的属性
* 参数：
*        [handle]   连接句柄
*        [base]     GOLDEN_POINT 结构，输入/输出，标签点基本属性。
                    输入时，由 id 字段指定要取得的可回收标签点。
*        [scan]     GOLDEN_SCAN_POINT 结构，输出，标签点采集扩展属性
*        [calc]     GOLDEN_CALC_POINT 结构，输出，标签点计算扩展属性
* 备注：scan、calc 可为空指针，对应的扩展信息将不返回。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_recycled_point_property(
                                golden_int32 handle, 
                                GOLDEN_POINT *base,
                                GOLDEN_SCAN_POINT *scan,
                                GOLDEN_CALC_POINT *calc
                                );

/**
* 命名：gob_get_recycled_max_point_property
* 功能：按最大长度获取可回收标签点的属性
* 参数：
*        [handle]   连接句柄
*        [base]     GOLDEN_POINT 结构，输入/输出，标签点基本属性。
                    输入时，由 id 字段指定要取得的可回收标签点。
*        [scan]     GOLDEN_SCAN_POINT 结构，输出，标签点采集扩展属性
*        [calc]     GOLDEN_MAX_CALC_POINT 结构，输出，标签点计算扩展属性
* 备注：scan、calc 可为空指针，对应的扩展信息将不返回。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_get_recycled_max_point_property(
																		golden_int32 handle,
																		GOLDEN_POINT* base,
																		GOLDEN_SCAN_POINT* scan,
																		GOLDEN_MAX_CALC_POINT* calc
																		);


/**
* 命名：gob_clear_recycler
* 功能：清空标签点回收站
* 参数：
*        [handle]   连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_clear_recycler(
                   golden_int32 handle 
                   );

/**
* 命名：gob_subscribe_tags
* 功能：批量标签点属性更改通知订阅
* 参数：
*        [handle]    连接句柄
*        [callback]  gob_tags_change 类型回调接口，输入，
*                    被订阅的标签点的属性值发生变化或被删除时将调用该接口通知用户,
*                    参见 golden.h 中原型的定义
* 备注：用于订阅测点的连接句柄必需是独立的，不能再用来调用其它 api，
*        否则返回 GoE_OTHER_SDK_DOING 错误。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_subscribe_tags(
                   golden_int32 handle,
                   gob_tags_change_event callback
                   );
/**
* 命名：gob_subscribe_tags_ex
* 功能：标签点属性更改通知订阅
* 参数：
*        [handle]    连接句柄
*        [options]   整型，输入，订阅选项，参见枚举GOLDEN_OPTION
*                    GOLDEN_O_AUTOCONN 订阅客户端与数据库服务器网络中断后自动重连并订阅
*        [param]     输入，用户参数，
*                    作为gob_tags_change_ex的param参数
*        [callback]  gob_tags_change_ex 类型回调接口，输入，当回掉函数返回非GoE_OK时退出订阅
*                    当未设置options为GOLDEN_O_AUTOCONN时，订阅断开后使用GOLDEN_E_DISCONNECT
*                    作为event_type取值调用回掉函数后退出订阅。
*                    当设置options为GOLDEN_O_AUTOCONN时，订阅断开后使用GOLDEN_E_DISCONNECT
*                    作为event_type取值调用回掉函数直到连接恢复或回掉函数返回非GoE_OK，
*                    网络中断期间回掉函数调用频率为最少3秒
*                    event_type参数值含义如下：
*                      GOLDEN_E_DATA        标签点属性发生更改
*                      GOLDEN_E_DISCONNECT  订阅客户端与数据库网络断开
*                      GOLDEN_E_RECOVERY    订阅客户端与数据库网络及订阅恢复
*                    handle 产生订阅回掉的连接句柄，调用gob_subscribe_tags_ex时的handle参数
*                    param  用户自定义参数，调用gob_subscribe_tags_ex时的param参数
*                    count  event_type为GOLDEN_E_DATA时表示ids的数量
*                           event_type为其它值时，count值为0
*                    ids    event_type为GOLDEN_E_DATA时表示属性更改的标签点ID，数量由count指定
*                           event_type为其它值时，ids值为NULL
*                    what   event_type为GOLDEN_E_DATA时表示属性变更原因，参考GOLDEN_TAG_CHANGE_REASON
*                           event_type为其它值时，what时值为0
* 备注：用于订阅测点的连接句柄必需是独立的，不能再用来调用其它 api，
*       否则返回 GoE_OTHER_SDK_DOING 错误。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_subscribe_tags_ex(
golden_int32 handle,
golden_uint32 options,
void* param,
gob_tags_change_event_ex callback
);
/**
* 命名：gob_cancel_subscribe_tags
* 功能：取消标签点属性更改通知订阅
* 参数：
*        [handle]    连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_cancel_subscribe_tags(
                          golden_int32 handle
                          );


/**
* 命名：gob_create_named_type
* 功能：创建自定义类型
* 参数：
*        [handle]      连接句柄，输入参数
*        [name]        自定义类型的名称，类型的唯一标示,不能重复，长度不能超过GOLDEN_TYPE_NAME_SIZE，输入参数
*        [field_count]    自定义类型中包含的字段的个数,输入参数
*        [fields]      自定义类型中包含的字段的属性，GOLDEN_DATA_TYPE_FIELD结构的数组，个数与field_count相等，输入参数
*              GOLDEN_DATA_TYPE_FIELD中的length只对type为str或blob类型的数据有效。其他类型忽略
* 备注：自定义类型的大小必须要小于数据页大小(小于数据页大小的2/3，即需要合理定义字段的个数及每个字段的长度)。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_create_named_type(
    golden_int32 handle,
    const char* name,
    golden_int32 field_count,
    const GOLDEN_DATA_TYPE_FIELD* fields,
    char desc[GOLDEN_DESC_SIZE]
    );

/**
* 命名：gob_get_named_types_count
* 功能：获取所有的自定义类型的总数
* 参数：
*        [handle]      连接句柄，输入参数
*        [count]      返回所有的自定义类型的总数，输入/输出参数
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_named_types_count(
    golden_int32 handle,
    golden_int32* count
    );

/**
* 命名：gob_get_all_named_types
* 功能：获取所有的自定义类型
* 参数：
*        [handle]      连接句柄，输入参数
*        [count]      返回所有的自定义类型的总数，输入/输出参数，输入:为name,field_counts数组的长度，输出:获取的实际自定义类型的个数
*        [name]        返回所有的自定义类型的名称的数组，每个自定义类型的名称的长度不超过GOLDEN_TYPE_NAME_SIZE，输入/输出参数
*              输入：name数组长度要等于count.输出：实际获取的自定义类型名称的数组
*        [field_counts]    返回所有的自定义类型所包含字段个数的数组，输入/输出参数
*              输入：field_counts数组长度要等于count。输出:实际每个自定义类型所包含的字段的个数的数组
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_all_named_types(
      golden_int32 handle,
      golden_int32* count,
      char* name[GOLDEN_TYPE_NAME_SIZE],
      golden_int32* field_counts
      );

/**
* 命名：gob_get_named_type
* 功能：获取自定义类型的所有字段
* 参数：
*        [handle]         连接句柄，输入参数
*        [name]           自定义类型的名称，输入参数
*        [field_count]    返回name指定的自定义类型的字段个数，输入/输出参数
*                         输入：指定fields数组长度.输出：实际的name自定义类型的字段的个数
*        [fields]         返回由name所指定的自定义类型所包含字段GOLDEN_DATA_TYPE_FIELD结构的数组，输入/输出参数
*                         输入：fields数组长度要等于count。输出:GOLDEN_DATA_TYPE_FIELD结构的数组
*        [type_size]      所有自定义类型fields结构中长度字段的累加和，输出参数
*        [desc]           自定义类型的描述，输出参数
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_get_named_type(
  golden_int32 handle,
  const char* name,
  golden_int32* field_count,
  GOLDEN_DATA_TYPE_FIELD* fields,
  golden_int32* type_size,
  char desc[GOLDEN_DESC_SIZE]
  );

/**
* 命名：gob_remove_named_type
* 功能：删除自定义类型
* 参数：
*        [handle]      连接句柄，输入参数
*        [name]        自定义类型的名称，输入参数
*        [reserved]      保留字段,暂时不用
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_remove_named_type(
    golden_int32 handle,
    const char* name,
    golden_int32 reserved=0
    );


GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
  gob_subscribe_named_type_create_event_ex(
  golden_int32 handle,
  golden_uint32 options,
  void* param,
  gob_named_type_create_event_ex callback
  );

/**
* 命名：gob_cancel_subscribe_named_type_create_event
* 功能：取消自定义类型创建事件订阅
* 参数：
*        [handle]    连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
  gob_cancel_subscribe_named_type_create_event(
  golden_int32 handle
  );

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gob_subscribe_named_type_remove_event_ex(
  golden_int32 handle,
  golden_uint32 options,
  void* param,
  gob_named_type_remove_event_ex callback
);

/**
* 命名：gob_cancel_subscribe_named_type_remove_event
* 功能：取消自定义类型删除事件订阅
* 参数：
*        [handle]    连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gob_cancel_subscribe_named_type_remove_event(
    golden_int32 handle
    );

/**
* 命名：gob_query_load_memory
* 功能：查询标签点属性加载到内存中的当前配置
* 参数：
*        [handle]   连接句柄
*        [load_memory_flag]     输出，加载到内存中的当前配置
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  gob_query_load_memory(golden_int32 handle,
  golden_int32 *load_memory_flag);

/**
* 命名：gob_set_load_memory
* 功能：设置标签点属性加载到内存中的配置
* 参数：
*        [handle]   连接句柄
*        [load_memory_flag]     输入/输出，输入时，表示要设置的加载到内存中的配置
*                                          输出时，表示成功加载到内存中的配置
* 备注：如果某些属性列成功加载到内存，某些属性列分配内存失败，就会返回GoE_OUT_OF_MEMORY，
*       此时还要根据返回的load_memory_flag来判断是不是全部分配失败。
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  gob_set_load_memory(
  golden_int32 handle ,
  golden_int32 *load_memory_flag);

/**
* 命名：gob_query_need_memory_size
* 功能：查询根据配置加载到内存中的标签点属性所占用的内存
* 参数：
*        [handle]   连接句柄
*        [load_memory_flag]     输入，计算需要多少内存的配置
*        [need_memory_size]     输出，需要占用内存的字节数，单位是byte
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  gob_query_need_memory_size(golden_int32 handle, golden_int32 *load_memory_flag, golden_int64* need_memory_size);

/**
* 命名：gob_get_named_type_names_property
* 功能：根据标签点id查询标签点所对应的自定义类型的名字和字段总数
* 参数：
*        [handle]           连接句柄
*        [count]            输入/输出，标签点个数，
*                           输入时表示 ids、named_type_names、field_counts、errors 的长度，
*                           输出时表示成功获取自定义类型名字的标签点个数
*        [ids]              整型数组，输入，标签点标识列表
*        [named_type_names] 字符串数组，输出，标签点自定义类型的名字
*        [field_counts]     整型数组，输出，标签点自定义类型的字段个数
*        [errors]           无符号整型数组，输出，获取自定义类型名字的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、named_type_names、field_counts、errors 的长度与 count 一致。
*        本接口只对数据类型为 GOLDEN_NAMED_T 的标签点有效。
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_named_type_names_property(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *named_type_names,
  golden_int32 *field_counts,
  golden_error *errors
  );

/**
* 命名：gob_get_recycled_named_type_names_property
* 功能：根据回收站标签点id查询标签点所对应的自定义类型的名字和字段总数
* 参数：
*        [handle]           连接句柄
*        [count]            输入/输出，标签点个数，
*                           输入时表示 ids、named_type_names、field_counts、errors 的长度，
*                           输出时表示成功获取自定义类型名字的标签点个数
*        [ids]              整型数组，输入，回收站标签点标识列表
*        [named_type_names] 字符串数组，输出，标签点自定义类型的名字
*        [field_counts]     整型数组，输出，标签点自定义类型的字段个数
*        [errors]           无符号整型数组，输出，获取自定义类型名字的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、named_type_names、field_counts、errors 的长度与 count 一致。
*        本接口只对数据类型为 GOLDEN_NAMED_T 的标签点有效。
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_recycled_named_type_names_property(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *named_type_names,
  golden_int32 *field_counts,
  golden_error *errors
  );

/**
* 命名：gob_get_named_type_points_count
* 功能：获取该自定义类型的所有标签点个数
* 参数：
*        [handle]           连接句柄，输入参数
*        [name]             自定义类型的名称，输入参数
*        [points_count]     返回name指定的自定义类型的标签点个数，输入参数
*/

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  gob_get_named_type_points_count(
  golden_int32 handle ,
  const char* name,
  golden_int32 *points_count);


/**
* 命名：gob_get_base_type_points_count
* 功能：获取该内置的基本类型的所有标签点个数
* 参数：
*        [handle]           整型，输入参数，连接句柄
*        [type]             整型，输入参数，内置的基本类型，参数的值可以是除GOLDEN_NAME_T以外的所有GOLDEN_TYPE枚举值
*        [points_count]     整型，输入参数，返回type指定的内置基本类型的标签点个数
*/
GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  gob_get_base_type_points_count(
  golden_int32 handle,
  golden_int32 type,
  golden_int32 *points_count
  );

/**
* 命名：gob_modify_named_type
* 功能：修改自定义类型名称,描述,字段名称,字段描述
* 参数：
*        [handle]             连接句柄，输入参数
*        [name]               自定义类型的名称，输入参数
*        [modify_name]        要修改的自定义类型名称，输入参数
*        [modify_desc]        要修改的自定义类型的描述，输入参数
*        [modify_field_name]  要修改的自定义类型字段的名称，输入参数
*        [modify_field_desc]  要修改的自定义类型字段的描述，输入参数
*        [field_count]        自定义类型字段的个数，输入参数
*/

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE
  gob_modify_named_type(
  golden_int32 handle ,
  const char* name,
  const char* modify_name,
  const char* modify_desc,
  const char* modify_field_name[GOLDEN_TYPE_NAME_SIZE],
  const char* modify_field_desc[GOLDEN_DESC_SIZE],
  golden_int32 field_count);

/************************************ 实时数据接口 ************************************/

/**
* 命名：gos_get_snapshots
* 功能：批量读取开关量、模拟量快照数值
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、values、states、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识列表
*        [datetimes] 整型数组，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [values]    双精度浮点型数组，输出，实时浮点型数值列表，
*                    对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，返回相应的快照值；否则为 0
*        [states]    64 位整型数组，输出，实时整型数值列表，
*                    对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                    GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，返回相应的快照值；否则为 0
*        [qualities] 短整型数组，输出，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、values、states、qualities、errors 的长度与 count 一致。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_STRING、GOLDEN_BLOB 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_snapshots(
                  golden_int32 handle,
                  golden_int32 *count, 
                  const golden_int32 *ids, 
                  golden_int32 *datetimes, 
                  golden_int16 *ms,
                  golden_float64 *values, 
                  golden_int64 *states, 
                  golden_int16 *qualities,
                  golden_error *errors
                  );

/**
* 命名：gos_put_snapshots
* 功能：批量写入开关量、模拟量快照数值
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、values、states、qualities、errors 的长度，
*                    输出时表示成功写入实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识列表，同一个标签点标识可以出现多次，
*                    但它们的时间戳必需是递增的。
*        [datetimes] 整型数组，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，表示相应的毫秒值；否则忽略
*        [values]    双精度浮点型数组，输入，实时浮点型数值列表，
*                    对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的快照值；否则忽略
*        [states]    64 位整型数组，输入，实时整型数值列表，
*                    对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                    GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的快照值；否则忽略
*        [qualities] 短整型数组，输入，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，写入实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、values、states、qualities、errors 的长度与 count 一致。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_STRING、GOLDEN_BLOB 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_put_snapshots(
                  golden_int32 handle,
                  golden_int32 *count, 
                  const golden_int32 *ids,
                  const golden_int32 *datetimes,
                  const golden_int16 *ms,
                  const golden_float64 *values,
                  const golden_int64 *states,
                  const golden_int16 *qualities,
                  golden_error *errors
                  );

/**
* 命名：gos_fix_snapshots
* 功能：批量写入开关量、模拟量快照数值
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、values、states、qualities、errors 的长度，
*                    输出时表示成功写入实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识列表，同一个标签点标识可以出现多次，
*                    但它们的时间戳必需是递增的。
*        [datetimes] 整型数组，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，表示相应的毫秒值；否则忽略
*        [values]    双精度浮点型数组，输入，实时浮点型数值列表，
*                    对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的快照值；否则忽略
*        [states]    64 位整型数组，输入，实时整型数值列表，
*                    对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                    GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的快照值；否则忽略
*        [qualities] 短整型数组，输入，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，写入实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、values、states、qualities、errors 的长度与 count 一致。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_STRING、GOLDEN_BLOB 的标签点无效。
*        仅当输入时间戳与当前快照时间戳完全相等时，会替换当前快照的值和质量；
*        其余情况下会调用 gos_put_snapshots()
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_fix_snapshots(
                  golden_int32 handle,
                  golden_int32 *count, 
                  const golden_int32 *ids,
                  const golden_int32 *datetimes,
                  const golden_int16 *ms,
                  const golden_float64 *values,
                  const golden_int64 *states,
                  const golden_int16 *qualities,
                  golden_error *errors
                  );

/**
* 命名：gos_get_coor_snapshots
* 功能：批量读取坐标实时数据
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、x、y、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识列表
*        [datetimes] 整型数组，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [x]         单精度浮点型数组，输出，实时浮点型横坐标数值列表
*        [y]         单精度浮点型数组，输出，实时浮点型纵坐标数值列表
*        [qualities] 短整型数组，输出，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、x、y、qualities、errors 的长度与 count 一致。
*        本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_coor_snapshots(
                       golden_int32 handle,
                       golden_int32 *count, 
                       const golden_int32 *ids, 
                       golden_int32 *datetimes,
                       golden_int16 *ms, 
                       golden_float32 *x, 
                       golden_float32 *y, 
                       golden_int16 *qualities,
                       golden_error *errors
                       );

/**
* 命名：gos_put_coor_snapshots
* 功能：批量写入坐标实时数据
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、x、y、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识列表
*        [datetimes] 整型数组，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [x]         单精度浮点型数组，输入，实时浮点型横坐标数值列表
*        [y]         单精度浮点型数组，输入，实时浮点型纵坐标数值列表
*        [qualities] 短整型数组，输入，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，写入实时坐标数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、x、y、qualities、errors 的长度与 count 一致。
*        本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_put_coor_snapshots(
                       golden_int32 handle,
                       golden_int32 *count,
                       const golden_int32 *ids,
                       const golden_int32 *datetimes, 
                       const golden_int16 *ms, 
                       const golden_float32 *x, 
                       const golden_float32 *y, 
                       const golden_int16 *qualities,
                       golden_error *errors
                       );

/**
* 命名：gos_fix_coor_snapshots
* 功能：批量写入坐标实时数据
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、x、y、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识列表
*        [datetimes] 整型数组，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [x]         单精度浮点型数组，输入，实时浮点型横坐标数值列表
*        [y]         单精度浮点型数组，输入，实时浮点型纵坐标数值列表
*        [qualities] 短整型数组，输入，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，写入实时坐标数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、x、y、qualities、errors 的长度与 count 一致。
*        本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*        仅当输入时间戳与当前快照时间戳完全相等时，会替换当前快照的值和质量；
*        其余情况下会调用 gos_put_coor_snapshots()
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_fix_coor_snapshots(
                       golden_int32 handle,
                       golden_int32 *count,
                       const golden_int32 *ids,
                       const golden_int32 *datetimes, 
                       const golden_int16 *ms, 
                       const golden_float32 *x, 
                       const golden_float32 *y, 
                       const golden_int16 *qualities,
                       golden_error *errors
                       );

/**
* 命名：gos_get_blob_snapshot
* 功能：读取二进制/字符串实时数据
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [blob]      字节型数组，输出，实时二进制/字符串数值
*        [len]       短整型，输入 / 输出，二进制/字符串数值长度
*        [quality]   短整型，输出，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_blob_snapshot(
                      golden_int32 handle,
                      golden_int32 id, 
                      golden_int32 *datetime,
                      golden_int16 *ms, 
                      golden_byte *blob, 
                      golden_int16 *len,
                      golden_int16 *quality
                      );

/**
* 命名：gos_get_blob_snapshots
* 功能：批量读取二进制/字符串实时数据
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、blobs、lens、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识
*        [datetimes] 整型数组，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [blobs]     字节型指针数组，输出，实时二进制/字符串数值
*        [lens]      短整型数组，输入/输出，二进制/字符串数值长度，
*                    输入时表示对应的 blobs 指针指向的缓冲区长度，
*                    输出时表示实际得到的 blob 长度，如果 blob 的长度大于缓冲区长度，会被截断。
*        [qualities] 短整型数组，输出，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_blob_snapshots(
                       golden_int32 handle,
                       golden_int32 *count, 
                       const golden_int32 *ids, 
                       golden_int32 *datetimes,
                       golden_int16 *ms, 
                       golden_byte* const* blobs, 
                       golden_int16 *lens,
                       golden_int16 *qualities, 
                       golden_error *errors 
                       );

/**
* 命名：gos_put_blob_snapshot
* 功能：写入二进制/字符串实时数据
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [blob]      字节型数组，输入，实时二进制/字符串数值
*        [len]       短整型，输入，二进制/字符串数值长度，超过一个页大小数据将被截断。
*        [quality]   短整型，输入，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_put_blob_snapshot(
                      golden_int32 handle,
                      golden_int32 id,
                      golden_int32 datetime, 
                      golden_int16 ms, 
                      const golden_byte *blob,
                      golden_int16 len,
                      golden_int16 quality
                      );

/**
* 命名：gos_put_blob_snapshots
* 功能：批量写入二进制/字符串实时数据
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、blobs、lens、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识
*        [datetimes] 整型数组，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，表示相应的毫秒值；否则忽略
*        [blobs]     字节型指针数组，输入，实时二进制/字符串数值
*        [lens]      短整型数组，输入，二进制/字符串数值长度，
*                    表示对应的 blobs 指针指向的缓冲区长度，超过一个页大小数据将被截断。
*        [qualities] 短整型数组，输入，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_put_blob_snapshots(
                       golden_int32 handle,
                       golden_int32 *count, 
                       const golden_int32 *ids, 
                       const golden_int32 *datetimes,
                       const golden_int16 *ms, 
                       const golden_byte* const* blobs, 
                       const golden_int16 *lens,
                       const golden_int16 *qualities, 
                       golden_error *errors 
                       );

/**
* 命名：gos_get_datetime_snapshots
* 功能：批量读取datetime类型标签点实时数据
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、dtvalues、dtlens、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识
*        [datetimes] 整型数组，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [dtvalues]  字节型指针数组，输出，实时datetime数值
*        [dtlens]    短整型数组，输入/输出，datetime数值长度，
*                    输入时表示对应的 dtvalues 指针指向的缓冲区长度，
*        [qualities] 短整型数组，输出，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
*        [type]      短整型，输入，所有标签点的显示类型，如“yyyy-mm-dd hh:mm:ss.000”的type为1，默认类型1，
*                    “yyyy/mm/dd hh:mm:ss.000”的type为2 
*                    如果不传type，则按照标签点属性显示，否则按照type类型显示
* 备注：本接口只对数据类型为 GOLDEN_DATETIME 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_datetime_snapshots(
                       golden_int32 handle,
                       golden_int32 *count, 
                       const golden_int32 *ids, 
                       golden_int32 *datetimes,
                       golden_int16 *ms, 
                       golden_byte* const* dtvalues , 
                       golden_int16 *dtlens,
                       golden_int16 *qualities, 
                       golden_error *errors,
                       golden_int16 type = -1
                       );

/**
* 命名：gos_put_datetime_snapshots
* 功能：批量插入datetime类型标签点数据
* 参数：
*        [handle]      连接句柄
*        [count]       整型，输入/输出，标签点个数，
*                      输入时表示 ids、datetimes、ms、dtvalues、dtlens、qualities、errors的长度，
*                      输出时表示成功写入的标签点个数
*        [ids]         整型数组，输入，标签点标识
*        [datatimes]   整型数组，输入，实时值时间列表
*                      表示距离1970年1月1日08:00:00的秒数
*        [ms]          短整型数组，输入，实时数值时间列表，
*                      对于时间精度为毫秒的标签点，表示相应的毫秒值；否则忽略
*        [dtvalues]    字节型指针数组，输入，datetime标签点的值
*        [dtlens]      短整型数组，输入，数值长度
*        [qualities]   短整型数组，输入，实时数值品质，，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]      无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
* 备注：被接口只对数据类型 GOLDEN_DATETIME 的标签点有效。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gos_put_datetime_snapshots(
                          golden_int32 handle,
                          golden_int32 *count, 
                          const golden_int32 *ids, 
                          const golden_int32 *datetimes,
                          const golden_int16 *ms, 
                          const golden_byte* const* dtvalues, 
                          const golden_int16 *dtlens,
                          const golden_int16 *qualities, 
                          golden_error *errors 
                          );


/**
* 命名：gos_subscribe_snapshots
* 功能：批量标签点快照改变的通知订阅
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，输入时表示 ids、errors 的长度，
*                    输出时表示成功订阅的标签点个数，不得超过 GOLDEN_MAX_SUBSCRIBE_SNAPSHOTS。
*        [ids]       整型数组，输入，标签点标识列表。
*        [callback]  gos_snaps_event 类型回调接口，输入，
*                    被订阅的标签点的快照值发生变化时将调用该接口，
*                    参见 golden.h 中原型的定义。
*        [errors]    无符号整型数组，输出，
*                    写入实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、errors 的长度与 count 一致。
*        用于订阅快照的连接句柄必需是独立的，不能再用来调用其它 api，
*        否则返回 GoE_OTHER_SDK_DOING 错误。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_subscribe_snapshots(
                        golden_int32 handle,
                        golden_int32 *count,
                        const golden_int32 *ids,
                        gos_snaps_event callback,
                        golden_error *errors
                        );

/**
* 命名：gos_subscribe_snapshots_ex
* 功能：批量标签点快照改变的通知订阅
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，输入时表示 ids、errors 的长度，
*                    输出时表示成功订阅的标签点个数，不得超过 GOLDEN_MAX_SUBSCRIBE_SNAPSHOTS。
*        [ids]       整型数组，输入，标签点标识列表。
*        [options]   整型，输入，订阅选项，参见枚举GOLDEN_OPTION
*                    GOLDEN_O_AUTOCONN 订阅客户端与数据库服务器网络中断后自动重连并订阅
*        [param]     输入，用户参数，
*                    作为gos_data_change_ex的param参数
*        [callback]  gos_snaps_event_ex 类型回调接口，输入，当回掉函数返回非GoE_OK时退出订阅
*                    当未设置options为GOLDEN_O_AUTOCONN时，订阅断开后使用GOLDEN_E_DISCONNECT
*                    作为event_type取值调用回掉函数后退出订阅。
*                    当设置options为GOLDEN_O_AUTOCONN时，订阅断开后使用GOLDEN_E_DISCONNECT
*                    作为event_type取值调用回掉函数直到连接恢复或回掉函数返回非GoE_OK，
*                    网络中断期间回掉函数调用频率为最少3秒
*                    event_type参数值含义如下：
*                      GOLDEN_E_DATA        标签点快照改变
*                      GOLDEN_E_DISCONNECT  订阅客户端与数据库网络断开
*                      GOLDEN_E_RECOVERY    订阅客户端与数据库网络及订阅恢复
*                    handle 产生订阅回掉的连接句柄，调用gos_subscribe_snapshots_ex时的handle参数
*                    param  用户自定义参数，调用gos_subscribe_snapshots_ex时的param参数
*                    count  event_type为GOLDEN_E_DATA时表示ids，datetimes,values等的数量
*                           event_type为其它值时，count值为0
*                    ids    event_type为GOLDEN_E_DATA时表示快照改变的标签点ID，数量由count指定
*                           event_type为其它值时，ids值为NULL
*                    datetimes event_type为GOLDEN_E_DATA时表示快照时间，数量由count指定
*                              event_type为其它值时，datetimes值为NULL
*                    ms     event_type为GOLDEN_E_DATA时表示快照的毫秒，数量由count指定
*                           event_type为其它值时，ms值为NULL
*                    values event_type为GOLDEN_E_DATA时表示浮点数据类型快照值，数量由count指定
*                           event_type为其它值时，values值为NULL
*                    states event_type为GOLDEN_E_DATA时表示整形数据类型快照值，数量由count指定
*                           event_type为其它值时，states值为NULL
*                    qualities event_type为GOLDEN_E_DATA时表示快照质量码，数量由count指定
*                           event_type为其它值时，qualities值为NULL
*                    errors event_type为GOLDEN_E_DATA时表示快照错误码，数量由count指定
*                           event_type为其它值时，errors值为NULL
*        [errors]    无符号整型数组，输出，
*                    写入实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、errors 的长度与 count 一致。
*        用于订阅快照的连接句柄必需是独立的，不能再用来调用其它 api，
*        否则返回 GoE_OTHER_SDK_DOING 错误。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
gos_subscribe_snapshots_ex(
golden_int32 handle,
golden_int32 *count,
const golden_int32 *ids,
golden_uint32 options,
void* param,
gos_snaps_event_ex callback,
golden_error *errors
);
/**
* 命名：gos_cancel_subscribe_snapshots
* 功能：取消标签点快照更改通知订阅
* 参数：
*        [handle]    连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_cancel_subscribe_snapshots(
                               golden_int32 handle
                               );

/**
* 命名：gos_get_named_type_snapshot
* 功能：获取自定义类型测点的单个快照
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [object]    字节型数组，输出，实时自定义类型标签点的数值
*        [length]    短整型，输入/输出，自定义类型标签点的数值长度
*        [quality]   短整型，输出，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_named_type_snapshot(
    golden_int32 handle,
    golden_int32 id, 
    golden_int32 *datetime,
    golden_int16 *ms, 
    void* object, 
    golden_int16 *length,
    golden_int16 *quality
    );

/**
* 命名：gos_get_named_type_snapshots
* 功能：批量获取自定义类型测点的快照
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、objects、lengths、qualities、errors 的长度，
*                    输出时表示成功获取实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识
*        [datetimes] 整型数组，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [objects]   指针数组，输出，自定义类型标签点数值
*        [lengths]   短整型数组，输入/输出，自定义类型标签点数值长度，
*                    输入时表示对应的 objects 指针指向的缓冲区长度，
*                    输出时表示实际得到的 objects 长度，如果 objects 的长度大于缓冲区长度，会被截断。
*        [qualities] 短整型数组，输出，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_get_named_type_snapshots(
    golden_int32 handle,
    golden_int32 *count, 
    const golden_int32 *ids, 
    golden_int32 *datetimes,
    golden_int16 *ms, 
    void* const* objects, 
    golden_int16 *lengths,
    golden_int16 *qualities, 
    golden_error *errors 
    );

/**
* 命名：gos_put_named_type_snapshot
* 功能：写入单个自定义类型标签点的快照
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [object]    void类型数组，输入，自定义类型标签点数值
*        [length]    短整型，输入，自定义类型标签点数值长度，超过一个页大小数据将被截断。
*        [quality]   短整型，输入，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_put_named_type_snapshot(
    golden_int32 handle,
    golden_int32 id,
    golden_int32 datetime, 
    golden_int16 ms, 
    const void *object,
    golden_int16 length,
    golden_int16 quality
    );

/**
* 命名：gos_put_named_type_snapshots
* 功能：批量写入自定义类型标签点的快照
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、datetimes、ms、objects、lengths、qualities、errors 的长度，
*                    输出时表示成功写入实时值的标签点个数
*        [ids]       整型数组，输入，标签点标识
*        [datetimes] 整型数组，输入，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，表示相应的毫秒值；否则忽略
*        [objects]   void类型指针数组，输入，自定义类型标签点数值
*        [lengths]   短整型数组，输入，自定义类型标签点数值长度，
*                    表示对应的 objects 指针指向的缓冲区长度，超过一个页大小数据将被截断。
*        [qualities] 短整型数组，输入，实时数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gos_put_named_type_snapshots(
    golden_int32 handle,
    golden_int32 *count, 
    const golden_int32 *ids, 
    const golden_int32 *datetimes,
    const golden_int16 *ms, 
    const void* const* objects, 
    const golden_int16 *lengths,
    const golden_int16 *qualities, 
    golden_error *errors 
    );
/************************************ 存档文件接口 ************************************/

/**
* 命名：goa_get_archives_count
* 功能：获取存档文件数量
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输出，存档文件数量
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_get_archives_count(
                       golden_int32 handle,
                       golden_int32 *count
                       );

/**
* 命名：goa_create_archive
* 功能：新建历史存档文件并追加到历史数据库
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名，文件后缀名应为.rdf。
*        [mb_size]    整型，输入，文件兆字节大小，单位为 MB。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_create_archive(
                   golden_int32 handle, 
                   const char *path,
                   const char *file,
                   golden_int32 mb_size
                   );

/**
* 命名：goa_create_ranged_archive
* 功能：新建指定时间范围的历史存档文件并插入到历史数据库
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*        [begin]      整数，输入，起始时间，距离1970年1月1日08:00:00的秒数
*        [end]        整数，输入，终止时间，距离1970年1月1日08:00:00的秒数
*        [mb_size]    整型，输入，文件兆字节大小，单位为 MB。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_create_ranged_archive(
                          golden_int32 handle,
                          const char *path, 
                          const char *file, 
                          golden_int32 begin, 
                          golden_int32 end, 
                          golden_int32 mb_size
                          );


/**
* 命名：goa_append_archive
* 功能：追加磁盘上的历史存档文件到历史数据库。
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名，后缀名应为.rdf。
*        [state]      整型，输入，取值 GOLDEN_ACTIVED_ARCHIVE、GOLDEN_NORMAL_ARCHIVE、
*                     GOLDEN_READONLY_ARCHIVE 之一，表示文件状态
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_append_archive(
                   golden_int32 handle,
                   const char *path,
                   const char *file, 
                   golden_int32 state
                   );

/**
* 命名：goa_remove_archive
* 功能：从历史数据库中移出历史存档文件。
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_remove_archive(
                   golden_int32 handle,
                   const char *path,
                   const char *file
                   );


/**
* 命名：goa_shift_actived
* 功能：切换活动文件
* 参数：
*        [handle]     连接句柄
* 备注：当前活动文件被写满时该事务被启动，
*        改变当前活动文件的状态为普通状态，
*        在所有历史数据存档文件中寻找未被使用过的
*        插入到前活动文件的右侧并改为活动状态，
*        若找不到则将前活动文件右侧的文件改为活动状态，
*        并将active_archive_指向该文件。该事务进行过程中，
*        用锁保证所有读写操作都暂停等待该事务完成。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_shift_actived(
                  golden_int32 handle
                  );

/**
* 命名：goa_reactive_archive
* 功能：激活指定存档文件为活动存档文件
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，存档文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，存档文件名。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_reactive_archive(
                     golden_int32 handle,
                     const char *path, 
                     const char *file
                     );

/**
* 命名：goa_get_first_archive
* 功能：获取首个存档文件的路径、名称、状态和最早允许写入时间。
* 参数：
*        [handle]          连接句柄
*        [path]            字符数组，输出，首个存档文件的目录路径，长度至少为 GOLDEN_PATH_SIZE。
*        [file]            字符数组，输出，首个存档文件的名称，长度至少为 GOLDEN_FILE_NAME_SIZE。
*        [state]           整型，输出，取值 GOLDEN_INVALID_ARCHIVE、GOLDEN_ACTIVED_ARCHIVE、
*                          GOLDEN_NORMAL_ARCHIVE、GOLDEN_READONLY_ARCHIVE 之一，表示文件状态
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
goa_get_first_archive(
                      golden_int32 handle,
                      char *path, 
                      char *file, 
                      golden_int32 *state
                      );

/**
* 命名：goa_get_next_archive
* 功能：获取下一个存档文件的路径、名称、状态和最早允许写入时间。
* 参数：
*        [handle]         连接句柄
*        [path]           字符数组，输入/输出，
*                         输入由调用 goa_get_first_archive 或
*                         上次调用 goa_get_next_archive 返回的文件目录路径，
*                         输出下一个存档文件的目录路径，长度至少为 GOLDEN_PATH_SIZE。
*        [file]           字符数组，输入/输出，
*                         输入由调用 goa_get_first_archive 或
*                         上次调用 goa_get_next_archive 返回的文件名，
*                         输出下一个存档文件的名称，长度至少为 GOLDEN_FILE_NAME_SIZE。
*        [state]          整型，输出，取值 GOLDEN_INVALID_ARCHIVE、GOLDEN_ACTIVED_ARCHIVE、
*                         GOLDEN_NORMAL_ARCHIVE、GOLDEN_READONLY_ARCHIVE 之一，表示文件状态
* 备注：当 path 返回内容为 "END" 时表示全部存档文件已经遍历完毕。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_get_next_archive(
                     golden_int32 handle,
                     char *path,
                     char *file,
                     golden_int32 *state
                     );


/**
* 命名：goa_get_archives
* 功能：获取存档文件的路径、名称、状态和最早允许写入时间。
* 参数：
*        [handle]          连接句柄
*        [paths]            字符串数组，输出，存档文件的目录路径，长度至少为 GOLDEN_PATH_SIZE。
*        [files]            字符串数组，输出，存档文件的名称，长度至少为 GOLDEN_FILE_NAME_SIZE。
*        [states]           整型数组，输出，取值 GOLDEN_INVALID_ARCHIVE、GOLDEN_ACTIVED_ARCHIVE、
*                          GOLDEN_NORMAL_ARCHIVE、GOLDEN_READONLY_ARCHIVE 之一，表示文件状态
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
goa_get_archives(
               golden_int32 handle,
               golden_int32* count,
               golden_path_string* paths, 
               golden_filename_string* files, 
               golden_int32 *states
               );


GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
goa_get_archives_info(
                     golden_int32 handle,
                     golden_int32* count,
                     const golden_path_string* const paths, 
                     const golden_filename_string* const files, 
                     GOLDEN_HEADER_PAGE *infos,
                     golden_error* errors
                     );

GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
goa_get_archives_perf_data(
                     golden_int32 handle,
                     golden_int32* count,
                     const golden_path_string* const paths, 
                     const golden_filename_string* const files, 
                     GOLDEN_ARCHIVE_PERF_DATA* real_time_datas,
                     GOLDEN_ARCHIVE_PERF_DATA* total_datas,
                     golden_error* errors
                     );

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goa_get_archives_status(golden_int32 handle, golden_error* status);

/**
* 命名：goa_get_archive_info
* 功能：获取存档文件及其附属文件的详细信息。
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*        [file_id]    整型，输入，附属文件标识，0 表示获取主文件信息。
*        [info]       GOLDEN_HEADER_PAGE 结构，输出，存档文件信息
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
goa_get_archive_info(
                     golden_int32 handle,
                     const char *path, 
                     const char *file, 
                     golden_int32 file_id, 
                     GOLDEN_HEADER_PAGE *info
                     );


/**
* 命名：goa_update_archive
* 功能：修改存档文件的可配置项。
* 参数：
*        [handle]         连接句柄
*        [path]           字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]           字符串，输入，文件名。
*        [rated_capacity] 整型，输入，文件额定大小，单位为 MB。
*        [ex_capacity]    整型，输入，附属文件大小，单位为 MB。
*        [auto_merge]     短整型，输入，是否自动合并附属文件。
*        [auto_arrange]   短整型，输入，是否自动整理存档文件。
* 备注: rated_capacity 与 ex_capacity 参数可为 0，表示不修改对应的配置项。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
goa_update_archive(
                   golden_int32 handle,
                   const char *path, 
                   const char *file, 
                   golden_int32 rated_capacity, 
                   golden_int32 ex_capacity, 
                   golden_int16 auto_merge, 
                   golden_int16 auto_arrange
                   );

/**
* 命名：goa_merge_archive
* 功能：合并附属文件到所属主文件
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，主文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，主文件名。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_merge_archive(
                  golden_int32 handle,
                  const char *path, 
                  const char *file
                  );

/**
* 命名：goa_arrange_archive
* 功能：整理存档文件，将同一标签点的数据块存放在一起以提高查询效率。
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_arrange_archive(
                    golden_int32 handle,
                    const char *path, 
                    const char *file
                    );

/**
* 命名：goa_reindex_archive
* 功能：为存档文件重新生成索引，用于恢复数据。
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*/
GOLDENAPI 
golden_error 
GOLDENAPI_CALLRULE 
goa_reindex_archive(
                    golden_int32 handle,
                    const char *path, 
                    const char *file
                    ); 

/**
* 命名：goa_backup_archive
* 功能：备份主存档文件及其附属文件到指定路径
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*        [dest]       字符串，输入，备份目录路径，必须以"\"或"/"结尾。
*/
GOLDENAPI 
golden_error 
GOLDENAPI_CALLRULE 
goa_backup_archive(
                   golden_int32 handle,
                   const char *path, 
                   const char *file, 
                   const char *dest
                   ); 

/**
* 命名：goa_move_archive
* 功能：将存档文件移动到指定目录
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*        [dest]       字符串，输入，移动目录路径，必须以"\"或"/"结尾。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goa_move_archive(
golden_int32 handle,
const char *path,
const char *file,
const char *dest
);

/**
* 命名：goa_reindex_archive
* 功能：为存档文件转换索引格式。
* 参数：
*        [handle]     连接句柄
*        [path]       字符串，输入，文件所在目录路径，必须以"\"或"/"结尾。
*        [file]       字符串，输入，文件名。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goa_convert_index(
golden_int32 handle,
const char *path,
const char *file
);


/**
* 命名：goa_query_big_job
* 功能：查询进程正在执行的后台任务类型、状态和进度
* 参数：
*        [handle]     连接句柄
*        [process]    所查询的进程代号，进程的标识参见枚举 GOLDEN_PROCESS_NAME, 
*                     GOLDEN_PROCESS_HISTORIAN: 历史服务进程，具有以下任务类型：
*                         GOLDEN_MERGE: 合并附属文件到主文件; 
*                         GOLDEN_ARRANGE: 整理存档文件; 
*                         GOLDEN_REINDEX: 重建索引; 
*                         GOLDEN_BACKUP: 备份; 
*                         GOLDEN_REACTIVE: 激活为活动存档; 
*                     GOLDEN_PROCESS_EQUATION: 方程式服务进程，具有以下任务类型：
*                         GOLDEN_COMPUTE: 历史计算; 
*                     GOLDEN_PROCESS_BASE: 标签信息服务进程，具有以下任务类型：
*                         GOLDEN_UPDATE_TABLE: 修改表名称; 
*                         GOLDEN_REMOVE_TABLE: 删除表; 
*        [path]       字符串，输出，长度至少为 GOLDEN_PATH_SIZE，
*                     对以下任务，这个字段表示存档文件所在目录路径：
*                         GOLDEN_MERGE
*                         GOLDEN_ARRANGE
*                         GOLDEN_REINDEX
*                         GOLDEN_BACKUP
*                         GOLDEN_REACTIVE
*                     对于以下任务，这个字段表示原来的表名：
*                         GOLDEN_UPDATE_TABLE
*                         GOLDEN_REMOVE_TABLE
*                     对于其它任务不可用。
*        [file]       字符串，输出，长度至少为 GOLDEN_FILE_NAME_SIZE，
*                     对以下任务，这个字段表示存档文件名：
*                         GOLDEN_MERGE
*                         GOLDEN_ARRANGE
*                         GOLDEN_REINDEX
*                         GOLDEN_BACKUP
*                         GOLDEN_REACTIVE
*                     对于以下任务，这个字段表示修改后的表名：
*                          GOLDEN_UPDATE_TABLE
*                     对于其它任务不可用。
*        [job]        短整型，输出，任务的标识参见枚举 GOLDEN_BIG_JOB_NAME。
*        [state]      整型，输出，任务的执行状态，参考 golden_error.h
*        [end_time]   整型，输出，任务的完成时间。
*        [progress]   单精度浮点型，输出，任务的进度百分比。
* 备注：path 及 file 参数可传空指针，对应的信息将不再返回。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goa_query_big_job(
                  golden_int32 handle,
                  golden_int32 process, 
                  char *path, 
                  char *file, 
                  golden_int16 *job, 
                  golden_int32 *state, 
                  golden_int32 *end_time, 
                  golden_float32 *progress 
                  );

/**
* 命名：goa_cancel_big_job
* 功能：取消进程正在执行的后台任务
* 参数：
*        [handle]     连接句柄
*        [process]    所查询的进程代号，进程的标识参见枚举 GOLDEN_PROCESS_NAME,
*                     GOLDEN_PROCESS_HISTORIAN: 历史服务进程，具有以下任务类型：
*                         GOLDEN_MERGE: 合并附属文件到主文件;
*                         GOLDEN_ARRANGE: 整理存档文件;
*                         GOLDEN_REINDEX: 重建索引;
*                         GOLDEN_BACKUP: 备份;
*                         GOLDEN_REACTIVE: 激活为活动存档;
* 备注：不支持取消    GOLDEN_PROCESS_EQUATION: 方程式服务进程
*                     GOLDEN_PROCESS_BASE: 标签信息服务进程
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goa_cancel_big_job(golden_int32 handle, golden_int32 process);



/************************************ 历史数据接口 ************************************/

/**
* 命名：goh_archived_values_count
* 功能：获取单个标签点在一段时间范围内的存储值数量.
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime1]     整型，输入，表示起始时间秒数。如果为 0，表示从存档中最早时间的数据开始读取
*        [ms1]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示起始时间对应的毫秒；否则忽略
*        [datetime2]     整型，输入，表示结束时间秒数。如果为 0，表示读取直至存档中数据的最后时间
*        [ms2]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示结束时间对应的毫秒；否则忽略
*        [count]         整型，输出，返回上述时间范围内的存储值数量
* 备注：由 datetime1、ms1 形成的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_archived_values_count(
                          golden_int32 handle,
                          golden_int32 id, 
                          golden_int32 datetime1, 
                          golden_int16 ms1, 
                          golden_int32 datetime2,
                          golden_int16 ms2, 
                          golden_int32 *count
                          );

/**
* 命名：goh_archived_values_real_count
* 功能：获取单个标签点在一段时间范围内的真实的存储值数量.
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime1]     整型，输入，表示起始时间秒数。如果为 0，表示从存档中最早时间的数据开始读取
*        [ms1]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示起始时间对应的毫秒；否则忽略
*        [datetime2]     整型，输入，表示结束时间秒数。如果为 0，表示读取直至存档中数据的最后时间
*        [ms2]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示结束时间对应的毫秒；否则忽略
*        [count]         整型，输出，返回上述时间范围内的存储值数量
* 备注：由 datetime1、ms1 形成的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*/

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goh_archived_values_real_count(
                              golden_int32 handle,
                              golden_int32 id, 
                              golden_int32 datetime1, 
                              golden_int16 ms1, 
                              golden_int32 datetime2,
                              golden_int16 ms2, 
                              golden_int32 *count
                              );

/**
* 命名：goh_get_archived_values
* 功能：读取单个标签点一段时间内的储存数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、values、states、qualities 的长度；
*                        输出时返回实际得到的数值个数
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，历史浮点型数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史存储值；否则为 0
*        [states]        64 位整数数组，输出，历史整型数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史存储值；否则为 0
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_archived_values(
                        golden_int32 handle,
                        golden_int32 id,
                        golden_int32 *count, 
                        golden_int32 *datetimes,
                        golden_int16 *ms, 
                        golden_float64 *values,
                        golden_int64 *states,
                        golden_int16 *qualities
                        );


/**
* 命名：goh_get_archived_values_backward
* 功能：逆向读取单个标签点一段时间内的储存数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、values、states、qualities 的长度；
*                        输出时返回实际得到的数值个数
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，历史浮点型数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史存储值；否则为 0
*        [states]        64 位整数数组，输出，历史整型数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史存储值；否则为 0
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_archived_values_backward(
                                 golden_int32 handle,
                                 golden_int32 id,
                                 golden_int32 *count, 
                                 golden_int32 *datetimes,
                                 golden_int16 *ms, 
                                 golden_float64 *values,
                                 golden_int64 *states,
                                 golden_int16 *qualities
                                 );

/**
* 命名：goh_get_archived_coor_values
* 功能：读取单个标签点一段时间内的坐标型储存数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、x、y、qualities 的长度；
*                        输出时返回实际得到的数值个数
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [x]             单精度浮点型数组，输出，浮点型横坐标历史数值列表
*        [y]             单精度浮点型数组，输出，浮点型纵坐标历史数值列表
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、x、y、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_archived_coor_values(
                             golden_int32 handle,
                             golden_int32 id,
                             golden_int32 *count,
                             golden_int32 *datetimes, 
                             golden_int16 *ms,
                             golden_float32 *x, 
                             golden_float32 *y, 
                             golden_int16 *qualities
                             );

/**
* 命名：goh_get_archived_coor_values_backward
* 功能：逆向读取单个标签点一段时间内的坐标型储存数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、x、y、qualities 的长度；
*                        输出时返回实际得到的数值个数
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [x]             单精度浮点型数组，输出，浮点型横坐标历史数值列表
*        [y]             单精度浮点型数组，输出，浮点型纵坐标历史数值列表
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、x、y、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_archived_coor_values_backward(
                                      golden_int32 handle,
                                      golden_int32 id,
                                      golden_int32 *count,
                                      golden_int32 *datetimes, 
                                      golden_int16 *ms,
                                      golden_float32 *x, 
                                      golden_float32 *y, 
                                      golden_int16 *qualities
                                      );


/**
* 命名：goh_get_archived_values_in_batches
* 功能：开始以分段返回方式读取一段时间内的储存数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime1]     整型，输入，表示起始时间秒数。如果为 0，表示从存档中最早时间的数据开始读取
*        [ms1]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示起始时间对应的毫秒；否则忽略
*        [datetime2]     整型，输入，表示结束时间秒数。如果为 0，表示读取直至存档中数据的最后时间
*        [ms2]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示结束时间对应的毫秒；否则忽略
*        [count]         整型，输出，返回上述时间范围内的存储值数量
*        [batch_count]   整型，输出，每次分段返回的长度，用于继续调用 goh_get_next_archived_values 接口
* 备注：由 datetime1、ms1 表示的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_archived_values_in_batches(
                                   golden_int32 handle,
                                   golden_int32 id,
                                   golden_int32 datetime1,
                                   golden_int16 ms1,
                                   golden_int32 datetime2,
                                   golden_int16 ms2,
                                   golden_int32 *count,
                                   golden_int32 *batch_count
                                   );

/**
* 命名：goh_get_next_archived_values
* 功能：分段读取一段时间内的储存数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整形，输入/输出，
*                        输入时表示 datetimes、ms、values、states、qualities 的长度；
*                        输出时表示实际得到的存储值个数。
*        [datetimes]     整型数组，输出，历史数值时间列表,
*                        表示距离1970年1月1日08:00:00的秒数
*        [ms]            短整型数组，输出，历史数值时间列表，
*                        对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [values]        双精度浮点型数组，输出，历史浮点型数值列表，
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，返回相应的历史存储值；否则为 0
*        [states]        64 位整型数组，输出，历史整型数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，返回相应的历史存储值；否则为 0
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 相符，
*        且 count 不能小于 goh_get_archived_values_in_batches 接口中返回的 batch_count 的值，
*        当返回 GoE_BATCH_END 表示全部数据获取完毕。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_next_archived_values(
                             golden_int32 handle,
                             golden_int32 id, 
                             golden_int32 *count, 
                             golden_int32 *datetimes, 
                             golden_int16 *ms, 
                             golden_float64 *values, 
                             golden_int64 *states,
                             golden_int16 *qualities
                             );

/**
* 命名：goh_get_timed_values
* 功能：获取单个标签点的单调递增时间序列历史插值。
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整型，输入，表示 datetimes、ms、values、states、qualities 的长度。
*        [datetimes]     整型数组，输入，表示需要的单调递增时间列表，
*                        为距离1970年1月1日08:00:00的秒数
*        [ms]            短整型数组，输入，对于时间精度为毫秒的标签点，
*                        表示需要的单调递增时间对应的毫秒值；否则忽略。
*        [values]        双精度浮点型数组，输出，历史浮点型数值列表，
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，返回相应的历史插值；否则为 0
*        [states]        64 位整型数组，输出，历史整型数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，返回相应的历史插值；否则为 0
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 相符，
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_timed_values(
                     golden_int32 handle, 
                     golden_int32 id, 
                     golden_int32 count, 
                     const golden_int32 *datetimes,
                     const golden_int16 *ms,
                     golden_float64 *values, 
                     golden_int64 *states,
                     golden_int16 *qualities
                     );

/**
* 命名：goh_get_timed_coor_values
* 功能：获取单个坐标标签点的单调递增时间序列历史插值。
* 参数：
*        [handle]        连接句柄
*        [pt]            整型，输入，标签点标识
*        [count]         整型，输入，表示 datetimes、ms、x、y、qualities 的长度。
*        [datetimes]     整型数组，输入，表示需要的单调递增时间列表，
*                        为距离1970年1月1日08:00:00的秒数
*        [ms]            短整型数组，输入，对于时间精度为毫秒的标签点，
*                        表示需要的单调递增时间对应的毫秒值；否则忽略。
*        [x]             单精度浮点型数组，输出，浮点型横坐标历史插值数值列表
*        [y]             单精度浮点型数组，输出，浮点型纵坐标历史插值数值列表
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、x、y、qualities 的长度与 count 相符，
*        本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_timed_coor_values(
                          golden_int32 handle, 
                          golden_int32 id,
                          golden_int32 count,
                          const golden_int32 *datetimes,
                          const golden_int16 *ms,
                          golden_float32 *x,
                          golden_float32 *y, 
                          golden_int16 *qualities
                          );

/**
* 命名：goh_get_interpo_values
* 功能：获取单个标签点一段时间内等间隔历史插值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、values、states、qualities 的长度，
*                        即需要的插值个数；输出时返回实际得到的插值个数
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史插值数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史插值；否则为 0
*        [states]        64 位整数数组，输出，整型历史插值数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史插值；否则为 0
*        [qualities]     短整型数组，输出，历史插值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_interpo_values(
                       golden_int32 handle,
                       golden_int32 id,
                       golden_int32 *count,
                       golden_int32 *datetimes, 
                       golden_int16 *ms , 
                       golden_float64 *values,
                       golden_int64 *states,
                       golden_int16 *qualities
                       );

/**
* 命名：goh_get_interval_values
* 功能：读取单个标签点某个时刻之后一定数量的等间隔内插值替换的历史数值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [interval]      整型，输入，插值时间间隔，单位为毫秒
*        [count]         整型，输入，表示 datetimes、ms、values、states、qualities 的长度，
*                        即需要的插值个数。
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史插值数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史插值；否则为 0
*        [states]        64 位整数数组，输出，整型历史插值数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史插值；否则为 0
*        [qualities]     短整型数组，输出，历史插值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素用于存放起始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_interval_values(
                        golden_int32 handle,
                        golden_int32 id, 
                        golden_int64 interval, 
                        golden_int32 count, 
                        golden_int32 *datetimes,
                        golden_int16 *ms,
                        golden_float64 *values,
                        golden_int64 *states,
                        golden_int16 *qualities
                        );

/**
* 命名：goh_get_single_value
* 功能：读取单个标签点某个时间的历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [mode]          整型，输入，取值 GOLDEN_NEXT、GOLDEN_PREVIOUS、GOLDEN_EXACT、GOLDEN_INTER 之一：
*                        GOLDEN_NEXT 寻找下一个最近的数据；
*                        GOLDEN_PREVIOUS 寻找上一个最近的数据；
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*                        GOLDEN_INTER 取指定时间的内插值数据。
*        [datetime]      整型，输入/输出，输入时表示时间秒数；
*                        输出时表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时表示时间毫秒数；输出时表示实际取得的历史数值时间毫秒数。
*                        否则忽略输入，输出时为 0。
*        [value]         双精度浮点数，输出，浮点型历史数值
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史值；否则为 0
*        [state]         64 位整数，输出，整型历史数值，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史值；否则为 0
*        [quality]       短整型，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_single_value(
                     golden_int32 handle,
                     golden_int32 id, 
                     golden_int32 mode, 
                     golden_int32 *datetime,
                     golden_int16 *ms,
                     golden_float64 *value, 
                     golden_int64 *state,
                     golden_int16 *quality
                     );

/**
* 命名：goh_get_single_coor_value
* 功能：读取单个标签点某个时间的坐标型历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [mode]          整型，输入，取值 GOLDEN_NEXT、GOLDEN_PREVIOUS、GOLDEN_EXACT、GOLDEN_INTER 之一：
*                        GOLDEN_NEXT 寻找下一个最近的数据；
*                        GOLDEN_PREVIOUS 寻找上一个最近的数据；
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*                        GOLDEN_INTER 取指定时间的内插值数据。
*        [datetime]      整型，输入/输出，输入时表示时间秒数；
*                        输出时表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时表示时间毫秒数；输出时表示实际取得的历史数值时间毫秒数。
*                        否则忽略输入，输出时为 0。
*        [x]             单精度浮点型，输出，横坐标历史数值
*        [y]             单精度浮点型，输出，纵坐标历史数值
*        [quality]       短整型，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_single_coor_value(
                          golden_int32 handle,
                          golden_int32 id,
                          golden_int32 mode,
                          golden_int32 *datetime,
                          golden_int16 *ms,
                          golden_float32 *x, 
                          golden_float32 *y, 
                          golden_int16 *quality 
                          );

/**
* 命名：goh_get_single_blob_value
* 功能：读取单个标签点某个时间的二进制/字符串型历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [mode]          整型，输入，取值 GOLDEN_NEXT、GOLDEN_PREVIOUS、GOLDEN_EXACT 之一：
*                        GOLDEN_NEXT 寻找下一个最近的数据；
*                        GOLDEN_PREVIOUS 寻找上一个最近的数据；
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*        [datetime]      整型，输入/输出，输入时表示时间秒数；
*                        输出时表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时表示时间毫秒数；输出时表示实际取得的历史数值时间毫秒数。
*                        否则忽略输入，输出时为 0。
*        [blob]          字节型数组，输出，二进制/字符串历史值
*        [len]           短整型，输入/输出，输入时表示 blob 的长度，
*                        输出时表示实际获取的二进制/字符串数据长度。
*        [quality]       短整型，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_single_blob_value(
                          golden_int32 handle,
                          golden_int32 id, 
                          golden_int32 mode, 
                          golden_int32 *datetime,
                          golden_int16 *ms, 
                          golden_byte *blob,
                          golden_int16 *len,
                          golden_int16 *quality
                          );


/**
* 命名：goh_get_archived_blob_values
* 功能：读取单个标签点段时间的二进制/字符串型历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*        [count]         整型，输入/输出，输入表示想要查询多少数据
*                        输出表示实际查到多少数据
*        [datetime1]     整型，输入，表示开始时间秒数；
*        [ms1]           短整型，输入，指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；
*        [datetime2]     整型，输入,表示结束时间秒数；
*        [ms2]           短整型，输入，指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；
*        [datetimes]     整型数组，输出，表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输出，如果 id 指定的标签点时间精度为毫秒，
*                        表示实际取得的历史数值时间毫秒数。
*        [lens]          短整型数组，输入/输出，输入时表示 blob 的长度，
*                        输出时表示实际获取的二进制/字符串数据长度。
*        [blobs]         字节型数组，输出，二进制/字符串历史值
*        [qualities]     短整型数组，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goh_get_archived_blob_values(
                             golden_int32 handle,
                             golden_int32 id,
                             golden_int32 *count,
                             golden_int32 datetime1,
                             golden_int16 ms1,
                             golden_int32 datetime2,
                             golden_int16 ms2,
                             golden_int32 *datetimes,
                             golden_int16 *ms,
                             golden_int16 *lens,
                             golden_byte* const* blobs,
                             golden_int16 *qualities
                             );

/**
* 命名：goh_get_single_datetime_value
* 功能：读取单个标签点某个时间的datetime历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [mode]          整型，输入，取值 GOLDEN_NEXT、GOLDEN_PREVIOUS、GOLDEN_EXACT 之一：
*                        GOLDEN_NEXT 寻找下一个最近的数据；
*                        GOLDEN_PREVIOUS 寻找上一个最近的数据；
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*        [datetime]      整型，输入/输出，输入时表示时间秒数；
*                        输出时表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时表示时间毫秒数；输出时表示实际取得的历史数值时间毫秒数。
*                        否则忽略输入，输出时为 0。
*        [blob]          字节型数组，输出，datetime历史值
*        [len]           短整型，输入/输出，输入时表示 blob 的长度，
*                        输出时表示实际获取的datetime数据长度。
*        [quality]       短整型，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_DATETIME 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_single_datetime_value(
                              golden_int32 handle,
                              golden_int32 id, 
                              golden_int32 mode, 
                              golden_int32 *datetime,
                              golden_int16 *ms, 
                              golden_byte *dtblob,
                              golden_int16 *dtlen,
                              golden_int16 *quality,
                              golden_int16 type = -1
                             );

/**
* 命名：goh_get_archived_datetime_values
* 功能：读取单个标签点一段时间的时间类型历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*        [count]         整型，输入/输出，输入表示想要查询多少数据
*                        输出表示实际查到多少数据
*        [datetime1]     整型，输入，表示开始时间秒数；
*        [ms1]           短整型，输入，指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；
*        [datetime2]     整型，输入,表示结束时间秒数；
*        [ms2]           短整型，输入，指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；
*        [datetimes]     整型数组，输出，表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输出，如果 id 指定的标签点时间精度为毫秒，
*                        表示实际取得的历史数值时间毫秒数。
*        [dtlens]          短整型数组，输入/输出，输入时表示 blob 的长度，
*                        输出时表示实际获取的世间数据长度。
*        [dtvalues]         字节型数组，输出，世间历史值
*        [qualities]     短整型数组，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [type]          短整型，输入，“yyyy-mm-dd hh:mm:ss.000”的type为1， 同样默认输入格式也为 “yyyy-mm-dd hh:mm:ss.000”
*                       “yyyy/mm/dd hh:mm:ss.000”的type为2 
* 备注：本接口只对数据类型为 GOLDEN_DATETIME 的标签点有效。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goh_get_archived_datetime_values(
                                golden_int32 handle,
                                golden_int32 id,
                                golden_int32 *count,
                                golden_int32 datetime1,
                                golden_int16 ms1,
                                golden_int32 datetime2,
                                golden_int16 ms2,
                                golden_int32 *datetimes,
                                golden_int16 *ms,
                                golden_int16 *lens,
                                golden_byte* const* blobs,
                                golden_int16 *qualities,
                                golden_int16 type = -1
                                );


/**
* 命名：goh_put_archived_datetime_values
* 功能：写入批量标签点批量时间型历史存储数据
* 参数：
*        [handle]        连接句柄
*        [count]         整型，输入/输出，
*                        输入时表示 ids、datetimes、ms、dtlens、dtvalues、qualities、errors 的长度，
*                        即历史值个数；输出时返回实际写入的数值个数
*        [ids]           整型数组，输入，标签点标识
*        [datetimes]     整型数组，输入，表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示对应的历史数值时间毫秒；否则忽略。
*        [dtvalues]      字节型指针数组，输入，实时时间数值
*        [dtlens]        短整型数组，输入，时间数值长度，
*                        表示对应的 dtvalues 指针指向的缓冲区长度，超过一个页大小数据将被截断。
*        [qualities]     短整型数组，输入，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]        无符号整型数组，输出，写入历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、dtlens、dtvalues、qualities、errors 的长度与 count 一致，
*        本接口仅对数据类型为 GOLDEN_DATETIME 的标签点有效。
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/

GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_archived_datetime_values(
                            golden_int32 handle, 
                            golden_int32 *count, 
                            const golden_int32 *ids,
                            const golden_int32 *datetimes, 
                            const golden_int16 *ms, 
                            const golden_byte* const* dtvalues, 
                            const golden_int16 *dtlens, 
                            const golden_int16 *qualities, 
                            golden_error* errors
                            );

/**
* 命名：goh_summary
* 功能：获取单个标签点一段时间内的统计值。
* 参数：
*        [handle]            连接句柄
*        [id]                整型，输入，标签点标识
*        [datetime1]         整型，输入/输出，输入时表示起始时间秒数。
*                            如果为 0，表示从存档中最早时间的数据开始进行统计。
*                            输出时返回最大值的时间秒数。
*        [ms1]               短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                            表示起始时间对应的毫秒，输出时表示最大值的时间毫秒数；否则忽略，返回值为 0 
*        [datetime2]         整型，输入/输出，输入时表示结束时间秒数。
*                            如果为 0，表示统计到存档中最近时间的数据为止。
*                            输出时返回最小值的时间秒数。
*        [ms2]               短整型，如果 id 指定的标签点时间精度为毫秒，
*                            表示结束时间对应的毫秒，输出时表示最小值的时间毫秒数；否则忽略，返回值为 0 
*        [max_value]         双精度浮点型，输出，表示统计时间段内的最大数值。
*        [min_value]         双精度浮点型，输出，表示统计时间段内的最小数值。
*        [total_value]       双精度浮点型，输出，表示统计时间段内的累计值，结果的单位为标签点的工程单位。
*        [calc_avg]          双精度浮点型，输出，表示统计时间段内的算术平均值。
*        [power_avg]         双精度浮点型，输出，表示统计时间段内的加权平均值。
* 备注：由 datetime1、ms1 表示的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*        如果输出的最大值或最小值的时间戳秒值为 0，
*        则表明仅有累计值和加权平均值输出有效，其余统计结果无效。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_summary(
            golden_int32 handle,
            golden_int32 id,
            golden_int32 *datetime1, 
            golden_int16 *ms1, 
            golden_int32 *datetime2, 
            golden_int16 *ms2, 
            golden_float64 *max_value,
            golden_float64 *min_value, 
            golden_float64 *total_value, 
            golden_float64 *calc_avg, 
            golden_float64 *power_avg
            );

/**
* 命名：goh_summary_ex
* 功能：获取单个标签点一段时间内的统计值，以及用于计算统计值的数据个数。
* 参数：
*        [handle]            连接句柄
*        [id]                整型，输入，标签点标识
*        [datetime1]         整型，输入/输出，输入时表示起始时间秒数。
*                            如果为 0，表示从存档中最早时间的数据开始进行统计。
*                            输出时返回最大值的时间秒数。
*        [ms1]               短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                            表示起始时间对应的毫秒，输出时表示最大值的时间毫秒数；否则忽略，返回值为 0
*        [datetime2]         整型，输入/输出，输入时表示结束时间秒数。
*                            如果为 0，表示统计到存档中最近时间的数据为止。
*                            输出时返回最小值的时间秒数。
*        [ms2]               短整型，如果 id 指定的标签点时间精度为毫秒，
*                            表示结束时间对应的毫秒，输出时表示最小值的时间毫秒数；否则忽略，返回值为 0
*        [max_value]         双精度浮点型，输出，表示统计时间段内的最大数值。
*        [min_value]         双精度浮点型，输出，表示统计时间段内的最小数值。
*        [total_value]       双精度浮点型，输出，表示统计时间段内的累计值，结果的单位为标签点的工程单位。
*        [calc_avg]          双精度浮点型，输出，表示统计时间段内的算术平均值。
*        [power_avg]         双精度浮点型，输出，表示统计时间段内的加权平均值。
*        [count]             整型，输出，表示统计时间段内用于计算统计值的数据个数。
* 备注：由 datetime1、ms1 表示的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*        如果输出的最大值或最小值的时间戳秒值为 0，
*        则表明仅有累计值和加权平均值输出有效，其余统计结果无效。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goh_summary_ex(
golden_int32 handle,
golden_int32 id,
golden_int32 *datetime1,
golden_int16 *ms1,
golden_int32 *datetime2,
golden_int16 *ms2,
golden_float64 *max_value,
golden_float64 *min_value,
golden_float64 *total_value,
golden_float64 *calc_avg,
golden_float64 *power_avg,
golden_int32 *count
);
/**
* 命名：goh_summary_in_batches
* 功能：分批获取单一标签点一段时间内的统计值
* 参数：
*        [handle]            连接句柄
*        [id]                整型，输入，标签点标识
*        [count]             整形，输入/输出，输入时表示 datatimes1、ms1、datatimes2、ms2、
*                            max_values、min_values、total_values、calc_avgs、power_avgs、errors 的长度，
*                            即分段的个数；输出时表示成功取得统计值的分段个数。
*        [interval]          64 位整型，输入，分段时间间隔，单位为毫秒。
*                            如果为毫秒点，输入时间必须大于1毫秒，如果为秒级点，则必须大于1000毫秒。
*        [datetimes1]        整型数组，输入/输出，输入时第一个元素表示起始时间秒数。
*                            如果为 0，表示从存档中最早时间的数据开始进行统计。
*                            输出时返回各个分段对应的最大值的时间秒数。
*        [ms1]               短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                            第一个元素表示起始时间对应的毫秒，
*                            输出时返回各个分段对应的最大值的时间毫秒数；否则忽略，返回值为 0 
*        [datetimes2]        整型数组，输入/输出，输入时第一个元素表示结束时间秒数。
*                            如果为 0，表示统计到存档中最近时间的数据为止。
*                            输出时返回各个分段对应的最小值的时间秒数。
*        [ms2]               短整型数组，如果 id 指定的标签点时间精度为毫秒，
*                            第一个元素表示结束时间对应的毫秒，
*                            输出时返回各个分段对应的最小值的时间毫秒数；否则忽略，返回值为 0 
*        [max_values]        双精度浮点型数组，输出，表示统计时间段内的最大数值。
*        [min_values]        双精度浮点型数组，输出，表示统计时间段内的最小数值。
*        [total_values]      双精度浮点型数组，输出，表示统计时间段内的累计值，结果的单位为标签点的工程单位。
*        [calc_avgs]         双精度浮点型数组，输出，表示统计时间段内的算术平均值。
*        [power_avgs]        双精度浮点型数组，输出，表示统计时间段内的加权平均值。
*        [errors]            无符号整型数组，输出，表示各个分段取得统计值的返回值。
* 备注：由 datetimes1[0]、ms1[0] 表示的时间可以大于 datetimes2[0]、ms2[0] 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*        如果输出的最大值或最小值的时间戳秒值为 0，
*        则表明仅有累计值和加权平均值输出有效，其余统计结果无效。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_summary_in_batches(
                       golden_int32 handle,
                       golden_int32 id, 
                       golden_int32 *count, 
                       golden_int64 interval,
                       golden_int32 *datetimes1,
                       golden_int16 *ms1, 
                       golden_int32 *datetimes2, 
                       golden_int16 *ms2,
                       golden_float64 *max_values, 
                       golden_float64 *min_values,
                       golden_float64 *total_values, 
                       golden_float64 *calc_avgs, 
                       golden_float64 *power_avgs,
                       golden_error *errors
                       );

/**
* 命名：goh_get_plot_values
* 功能：获取单个标签点一段时间内用于绘图的历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [interval]      整型，输入，时间区间数量，单位为个，
*                        一般会使用绘图的横轴(时间轴)所用屏幕像素数，
*                        该功能将起始至结束时间等分为 interval 个区间，
*                        并返回每个区间的第一个和最后一个数值、最大和最小数值、一条异常数值；
*                        故参数 count 有可能输出五倍于 interval 的历史值个数，
*                        所以推荐输入的 count 至少是 interval 的五倍。
*        [count]         整型，输入/输出，输入时表示 datetimes、ms、values、states、qualities 的长度，
*                        即需要获取的最大历史值个数，输出时返回实际得到的历史值个数。
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史值数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史值；否则为 0
*        [states]        64 位整数数组，输出，整型历史值数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史值；否则为 0
*        [qualities]     短整型数组，输出，历史值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，用以存放起始及结束时间。
*        第一个元素形成的时间可以大于最后一个元素形成的时间，
*        此时第一个元素表示结束时间，最后一个元素表示开始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_plot_values(
                    golden_int32 handle, 
                    golden_int32 id,
                    golden_int32 interval,
                    golden_int32 *count, 
                    golden_int32 *datetimes, 
                    golden_int16 *ms,
                    golden_float64 *values, 
                    golden_int64 *states,
                    golden_int16 *qualities
                    );

/**
* 命名：goh_get_cross_section_values
* 功能：获取批量标签点在某一时间的历史断面数据
* 参数：
*        [handle]        连接句柄
*        [ids]           整型数组，输入，标签点标识列表
*        [mode]          整型，输入，取值 GOLDEN_NEXT、GOLDEN_PREVIOUS、GOLDEN_EXACT、GOLDEN_INTER 之一：
*                        GOLDEN_NEXT 寻找下一个最近的数据；
*                        GOLDEN_PREVIOUS 寻找上一个最近的数据；
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*                        GOLDEN_INTER 取指定时间的内插值数据。
*        [count]         整型，输入，表示 ids、datetimes、ms、values、states、qualities 的长度，即标签点个数。
*        [datatimes]     整型数组，输入/输出，输入时表示对应标签点的历史数值时间秒数，
*                        输出时表示根据 mode 实际寻找到的数值时间秒数。
*        [ms]            短整型数组，输入/输出，对于时间精度为毫秒的标签点，
*                        输入时表示历史数值时间毫秒数，存放相应的毫秒值，
*                        输出时表示根据 mode 实际寻找到的数值时间毫秒数；否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史值数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史值；否则为 0
*        [states]        64 位整数数组，输出，整型历史值数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史值；否则为 0
*        [qualities]     短整型数组，输出，历史值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]        无符号整型数组，输出，读取历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、values、states、qualities 的长度与 count 一致，
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_cross_section_values(
                             golden_int32 handle,
                             const golden_int32 *ids,
                             golden_int32 mode, 
                             golden_int32 count,
                             golden_int32 *datetimes, 
                             golden_int16 *ms, 
                             golden_float64 *values, 
                             golden_int64 *states, 
                             golden_int16 *qualities,
                             golden_error *errors
                             );

/**
* 命名：goh_subscribe_playback
* 功能：订阅一批标签点的历史数据回放
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、errors 的长度，
*                    输出时表示成功订阅的标签点个数
*        [ids]       整型数组，输入，标签点标识列表
*        [datetime1] 整型，输入，回放时间段起始时间。
*        [datetime2] 整型，输入，回放时间段结束时间。
*        [interval]  整型，输入，每两次接收到的数据之间的时间跨度，单位：秒。
*        [callback]  goh_data_playback 类型回调接口，输入，
*                    通过该接口获得标签点的回放数据。
*        [errors]    无符号整型数组，输出，
*                    订阅是否成功的列表，参考golden_error.h
* 备注：用户须保证 ids、errors 的长度与 count 一致。
*        用于订阅数据回放的连接句柄必需是独立的，不能再用来调用其它 api，
*        否则返回 GoE_OTHER_SDK_DOING 错误。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_subscribe_playback(
                       golden_int32 handle,
                       golden_int32 *count,
                       const golden_int32 *ids,
                       golden_int32 datetime1,
                       golden_int32 datetime2,
                       golden_int32 interval,
                       goh_data_playback callback,
                       golden_error *errors
                       );
/**
* 命名：goh_subscribe_playback
* 功能：订阅一批标签点的历史数据回放
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，标签点个数，
*                    输入时表示 ids、errors 的长度，
*                    输出时表示成功订阅的标签点个数
*        [ids]       整型数组，输入，标签点标识列表
*        [datetime1] 整型，输入，回放时间段起始时间。
*        [datetime2] 整型，输入，回放时间段结束时间。
*        [interval]  整型，输入，每两次接收到的数据之间的时间跨度，单位：秒。
*        [callback]  goh_data_playback 类型回调接口，输入，
*                    通过该接口获得标签点的回放数据。
*        [errors]    无符号整型数组，输出，
*                    订阅是否成功的列表，参考golden_error.h
* 备注：用户须保证 ids、errors 的长度与 count 一致。
*        用于订阅数据回放的连接句柄必需是独立的，不能再用来调用其它 api，
*        否则返回 GoE_OTHER_SDK_DOING 错误。
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goh_subscribe_playback_ex(
golden_int32 handle,
golden_int32 *count,
const golden_int32 *ids,
golden_int32 datetime1,
golden_int32 datetime2,
golden_int32 interval,
goh_data_playback_ex callback,
golden_error *errors
);

/**
* 命名：goh_stop_playback
* 功能：停止历史数据回放
* 参数：
*        [handle]    连接句柄
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_stop_playback(
                  golden_int32 handle
                  );

/**
* 命名：goh_get_archived_values_filt
* 功能：读取单个标签点在一段时间内经复杂条件筛选后的历史储存值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [filter]        字符串，输入，由算术、逻辑运算符组成的复杂条件表达式，
*                        长度不得超过 GOLDEN_EQUATION_SIZE，为 0 则不进行条件筛选。
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、values、states、qualities 的长度，
*                        即需要的数值个数；输出时返回实际得到的数值个数。
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史存储值；否则为 0
*        [states]        64 位整数数组，输出，整型历史数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史存储值；否则为 0
*        [qualities]     短整型数组，输出，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_archived_values_filt(
                             golden_int32 handle,
                             golden_int32 id,
                             const char *filter,
                             golden_int32 *count,
                             golden_int32 *datetimes,
                             golden_int16 *ms,
                             golden_float64 *values, 
                             golden_int64 *states,
                             golden_int16 *qualities
                             );

/**
* 命名：goh_get_interval_values_filt
* 功能：读取单个标签点某个时刻之后经复杂条件筛选后一定数量的等间隔内插值替换的历史数值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [filter]        字符串，输入，由算术、逻辑运算符组成的复杂条件表达式，
*                        长度不得超过 GOLDEN_EQUATION_SIZE，长度为 0 则不进行条件筛选。
*        [interval]      整型，输入，插值时间间隔，单位为毫秒
*        [count]         整型，输入，表示 datetimes、ms、values、states、qualities 的长度，
*                        即需要的插值个数。
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史插值数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史插值；否则为 0
*        [states]        64 位整数数组，输出，整型历史插值数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史插值；否则为 0
*        [qualities]     短整型数组，输出，历史插值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素用于表示起始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_interval_values_filt(
                             golden_int32 handle,
                             golden_int32 id,
                             const char *filter,
                             golden_int64 interval,
                             golden_int32 count,
                             golden_int32 *datetimes,
                             golden_int16 *ms, 
                             golden_float64 *values,
                             golden_int64 *states,
                             golden_int16 *qualities
                             );

/**
* 命名：goh_get_interpo_values_filt
* 功能：获取单个标签点一段时间内经复杂条件筛选后的等间隔插值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [filter]        字符串，输入，由算术、逻辑运算符组成的复杂条件表达式，
*                        长度不得超过 GOLDEN_EQUATION_SIZE，长度为 0 则不进行条件筛选。
*        [count]         整型，输入/输出，
*                        输入时表示 datetimes、ms、values、states、qualities 的长度，
*                        即需要的插值个数；输出时返回实际得到的插值个数
*        [datetimes]     整型数组，输入/输出，
*                        输入时第一个元素表示起始时间秒数，
*                        最后一个元素表示结束时间秒数，如果为 0，表示直到数据的最后时间；
*                        输出时表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时第一个元素表示起始时间毫秒，
*                        最后一个元素表示结束时间毫秒；
*                        输出时表示对应的历史数值时间毫秒。
*                        否则忽略输入，输出时为 0。
*        [values]        双精度浮点数数组，输出，浮点型历史插值数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的历史插值；否则为 0
*        [states]        64 位整数数组，输出，整型历史插值数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的历史插值；否则为 0
*        [qualities]     短整型数组，输出，历史插值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：用户须保证 datetimes、ms、values、states、qualities 的长度与 count 一致，
*        在输入时，datetimes、ms 中至少应有一个元素，第一个元素形成的时间可以
*        大于最后一个元素形成的时间，此时第一个元素表示结束时间，
*        最后一个元素表示开始时间。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_interpo_values_filt(
                            golden_int32 handle,
                            golden_int32 id,
                            const char *filter,
                            golden_int32 *count, 
                            golden_int32 *datetimes, 
                            golden_int16 *ms, 
                            golden_float64 *values, 
                            golden_int64 *states,
                            golden_int16 *qualities
                            );

/**
* 命名：goh_summary_filt
* 功能：获取单个标签点一段时间内经复杂条件筛选后的统计值
* 参数：
*        [handle]            连接句柄
*        [id]                整型，输入，标签点标识
*        [filter]            字符串，输入，由算术、逻辑运算符组成的复杂条件表达式，
*                            长度不得超过 GOLDEN_EQUATION_SIZE，长度为 0 则不进行条件筛选。
*        [datetime1]         整型，输入/输出，输入时表示起始时间秒数。
*                            如果为 0，表示从存档中最早时间的数据开始进行统计。
*                            输出时返回最大值的时间秒数。
*        [ms1]               短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                            表示起始时间对应的毫秒，输出时表示最大值的时间毫秒数；否则忽略，返回值为 0 
*        [datetime2]         整型，输入/输出，输入时表示结束时间秒数。
*                            如果为 0，表示统计到存档中最近时间的数据为止。
*                            输出时返回最小值的时间秒数。
*        [ms2]               短整型，如果 id 指定的标签点时间精度为毫秒，
*                            表示结束时间对应的毫秒，输出时表示最小值的时间毫秒数；否则忽略，返回值为 0 
*        [max_value]         双精度浮点型，输出，表示统计时间段内的最大数值。
*        [min_value]         双精度浮点型，输出，表示统计时间段内的最小数值。
*        [total_value]       双精度浮点型，输出，表示统计时间段内的累计值，结果的单位为标签点的工程单位。
*        [calc_avg]          双精度浮点型，输出，表示统计时间段内的算术平均值。
*        [power_avg]         双精度浮点型，输出，表示统计时间段内的加权平均值。
* 备注：由 datetime1、ms1 表示的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*        如果输出的最大值或最小值的时间戳秒值为 0，
*        则表明仅有累计值和加权平均值输出有效，其余统计结果无效。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_summary_filt(
                 golden_int32 handle,
                 golden_int32 id,
                 const char *filter,
                 golden_int32 *datetime1, 
                 golden_int16 *ms1, 
                 golden_int32 *datetime2, 
                 golden_int16 *ms2, 
                 golden_float64 *max_value, 
                 golden_float64 *min_value, 
                 golden_float64 *total_value,
                 golden_float64 *calc_avg, 
                 golden_float64 *power_avg
                 );

/**
* 命名：goh_summary_filt_in_batches
* 功能：分批获取单一标签点一段时间内经复杂条件筛选后的统计值
* 参数：
*        [handle]            连接句柄
*        [id]                整型，输入，标签点标识
*        [filter]            字符串，输入，由算术、逻辑运算符组成的复杂条件表达式，
*                            长度不得超过 GOLDEN_EQUATION_SIZE，长度为 0 则不进行条件筛选。
*        [count]             整形，输入/输出，输入时表示 datatimes1、ms1、datatimes2、ms2、
*                            max_values、min_values、total_values、calc_avgs、power_avgs、errors 的长度，
*                            即分段的个数；输出时表示成功取得统计值的分段个数。
*        [interval]          64 位整型，输入，分段时间间隔，单位为毫秒。
*        [datetimes1]        整型数组，输入/输出，输入时第一个元素表示起始时间秒数。
*                            如果为 0，表示从存档中最早时间的数据开始进行统计。
*                            输出时返回各个分段对应的最大值的时间秒数。
*        [ms1]               短整型数组，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                            第一个元素表示起始时间对应的毫秒，
*                            输出时返回各个分段对应的最大值的时间毫秒数；否则忽略，返回值为 0 
*        [datetimes2]        整型数组，输入/输出，输入时第一个元素表示结束时间秒数。
*                            如果为 0，表示统计到存档中最近时间的数据为止。
*                            输出时返回各个分段对应的最小值的时间秒数。
*        [ms2]               短整型数组，如果 id 指定的标签点时间精度为毫秒，
*                            第一个元素表示结束时间对应的毫秒，
*                            输出时返回各个分段对应的最小值的时间毫秒数；否则忽略，返回值为 0 
*        [max_values]        双精度浮点型数组，输出，表示统计时间段内的最大数值。
*        [min_values]        双精度浮点型数组，输出，表示统计时间段内的最小数值。
*        [total_values]      双精度浮点型数组，输出，表示统计时间段内的累计值，结果的单位为标签点的工程单位。
*        [calc_avgs]         双精度浮点型数组，输出，表示统计时间段内的算术平均值。
*        [power_avgs]        双精度浮点型数组，输出，表示统计时间段内的加权平均值。
*        [errors]            无符号整型数组，输出，表示各个分段取得统计值的返回值。
* 备注：由 datetimes1[0]、ms1[0] 表示的时间可以大于 datetimes2[0]、ms2[0] 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*        如果输出的最大值或最小值的时间戳秒值为 0，
*        则表明仅有累计值和加权平均值输出有效，其余统计结果无效。
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_summary_filt_in_batches(
                            golden_int32 handle,
                            golden_int32 id, 
                            const char *filter,
                            golden_int32 *count, 
                            golden_int64 interval,
                            golden_int32 *datetimes1,
                            golden_int16 *ms1, 
                            golden_int32 *datetimes2, 
                            golden_int16 *ms2,
                            golden_float64 *max_values, 
                            golden_float64 *min_values,
                            golden_float64 *total_values, 
                            golden_float64 *calc_avgs, 
                            golden_float64 *power_avgs,
                            golden_error *errors
                            );

/**
* 命名：goh_update_value
* 功能：修改单个标签点某一时间的历史存储值.
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime]      整型，输入，时间秒数
*        [ms]            短整型，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；否则忽略。
*        [value]         双精度浮点数，输入，浮点型历史数值
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放新的历史值；否则忽略
*        [state]         64 位整数，输入，整型历史数值，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放新的历史值；否则忽略
*        [quality]       短整型，输入，新的历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_update_value(
                 golden_int32 handle,
                 golden_int32 id, 
                 golden_int32 datetime, 
                 golden_int16 ms, 
                 golden_float64 value, 
                 golden_int64 state,
                 golden_int16 quality
                 );

/**
* 命名：goh_update_coor_value
* 功能：修改单个标签点某一时间的历史存储值.
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime]      整型，输入，时间秒数
*        [ms]            短整型，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；否则忽略。
*        [x]             单精度浮点型，输入，新的横坐标历史数值
*        [y]             单精度浮点型，输入，新的纵坐标历史数值
*        [quality]       短整型，输入，新的历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口仅对数据类型为 GOLDEN_COOR 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_update_coor_value(
                      golden_int32 handle,
                      golden_int32 id,
                      golden_int32 datetime,
                      golden_int16 ms,
                      golden_float32 x,
                      golden_float32 y,
                      golden_int16 quality
                      );


/**
* 命名：goh_remove_value
* 功能：删除单个标签点某个时间的历史存储值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime]      整型，输入，时间秒数
*        [ms]            短整型，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；否则忽略。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_remove_value(
                 golden_int32 handle,
                 golden_int32 id,
                 golden_int32 datetime,
                 golden_int16 ms
                 );

/**
* 命名：goh_remove_values
* 功能：删除单个标签点一段时间内的历史存储值
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime1]     整型，输入，表示起始时间秒数。如果为 0，表示从存档中最早时间的数据开始读取
*        [ms1]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示起始时间对应的毫秒；否则忽略
*        [datetime2]     整型，输入，表示结束时间秒数。如果为 0，表示读取直至存档中数据的最后时间
*        [ms2]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示结束时间对应的毫秒；否则忽略
*        [count]         整形，输出，表示删除的历史值个数
* 备注：由 datetime1、ms1 表示的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_remove_values(
                  golden_int32 handle,
                  golden_int32 id,
                  golden_int32 datetime1, 
                  golden_int16 ms1,
                  golden_int32 datetime2, 
                  golden_int16 ms2,
                  golden_int32 *count
                  );


/**
* 命名：goh_put_single_value
* 功能：写入单个标签点在某一时间的历史数据。
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime]      整型，输入，时间秒数
*        [ms]            短整型，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；否则忽略。
*        [value]         双精度浮点数，输入，浮点型历史数值
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放历史值；否则忽略
*        [state]         64 位整数，输入，整型历史数值，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放历史值；否则忽略
*        [quality]       短整型，输入，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_single_value(
                     golden_int32 handle, 
                     golden_int32 id,
                     golden_int32 datetime, 
                     golden_int16 ms,
                     golden_float64 value, 
                     golden_int64 state,
                     golden_int16 quality
                     );

/**
* 命名：goh_put_single_coor_value
* 功能：写入单个标签点在某一时间的坐标型历史数据。
* 参数：
*        [handle]              连接句柄
*        [id]            整型，输入，标签点标识
*        [datetime]      整型，输入，时间秒数
*        [ms]            短整型，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；否则忽略。
*        [x]             单精度浮点型，输入，横坐标历史数值
*        [y]             单精度浮点型，输入，纵坐标历史数值
*        [quality]       短整型，输入，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_single_coor_value(
                          golden_int32 handle, 
                          golden_int32 id, 
                          golden_int32 datetime, 
                          golden_int16 ms, 
                          golden_float32 x, 
                          golden_float32 y, 
                          golden_int16 quality
                          );

/**
* 命名：goh_put_single_blob_value
* 功能：写入单个二进制/字符串标签点在某一时间的历史数据
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输入，数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输入，历史数值时间，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [blob]      字节型数组，输入，历史二进制/字符串数值
*        [len]       短整型，输入，二进制/字符串数值长度，超过一个页大小数据将被截断。
*        [quality]   短整型，输入，历史数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_BLOB、GOLDEN_STRING 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_single_blob_value(
                          golden_int32 handle,
                          golden_int32 id,
                          golden_int32 datetime, 
                          golden_int16 ms, 
                          const golden_byte *blob,
                          golden_int16 len,
                          golden_int16 quality
                          );

/**
* 命名：goh_put_archived_values
* 功能：写入批量标签点批量历史存储数据
* 参数：
*        [handle]        连接句柄
*        [count]         整型，输入/输出，
*                        输入时表示 ids、datetimes、ms、values、states、qualities、errors 的长度，
*                        即历史值个数；输出时返回实际写入的数值个数
*        [ids]           整型数组，输入，标签点标识，同一个标签点标识可以出现多次，
*                        但它们的时间戳必需是递增的。
*        [datetimes]     整型数组，输入，表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示对应的历史数值时间毫秒；否则忽略。
*        [values]        双精度浮点数数组，输入，浮点型历史数值列表
*                        对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，表示相应的历史存储值；否则忽略
*        [states]        64 位整数数组，输入，整型历史数值列表，
*                        对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                        GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，表示相应的历史存储值；否则忽略
*        [qualities]     短整型数组，输入，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]        无符号整型数组，输出，写入历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、values、states、qualities、errors 的长度与 count 一致，
*        本接口对数据类型为 GOLDEN_COOR、GOLDEN_BLOB、GOLDEN_STRING 的标签点无效。
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_archived_values(
                        golden_int32 handle, 
                        golden_int32 *count, 
                        const golden_int32 *ids, 
                        const golden_int32 *datetimes,
                        const golden_int16 *ms, 
                        const golden_float64 *values,
                        const golden_int64 *states,
                        const golden_int16 *qualities,
                        golden_error *errors
                        );

/**
* 命名：goh_put_archived_coor_values
* 功能：写入批量标签点批量坐标型历史存储数据
* 参数：
*        [handle]        连接句柄
*        [count]         整型，输入/输出，
*                        输入时表示 ids、datetimes、ms、x、y、qualities、errors 的长度，
*                        即历史值个数；输出时返回实际写入的数值个数
*        [ids]           整型数组，输入，标签点标识
*        [datetimes]     整型数组，输入，表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示对应的历史数值时间毫秒；否则忽略。
*        [x]             单精度浮点型数组，输入，浮点型横坐标历史数值列表
*        [y]             单精度浮点型数组，输入，浮点型纵坐标历史数值列表
*        [qualities]     短整型数组，输入，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]        无符号整型数组，输出，写入历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、x、y、qualities、errors 的长度与 count 一致，
*        本接口仅对数据类型为 GOLDEN_COOR 的标签点有效。
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_archived_coor_values(
                             golden_int32 handle, 
                             golden_int32 *count, 
                             const golden_int32 *ids, 
                             const golden_int32 *datetimes, 
                             const golden_int16 *ms, 
                             const golden_float32 *x, 
                             const golden_float32 *y, 
                             const golden_int16 *qualities,
                             golden_error *errors
                             );

/**
* 命名：goh_put_single_datetime_value
* 功能：写入单个datetime标签点在某一时间的历史数据
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输入，数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输入，历史数值时间，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [blob]      字节型数组，输入，历史datetime数值
*        [len]       短整型，输入，datetime数值长度，超过一个页大小数据将被截断。
*        [quality]   短整型，输入，历史数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
* 备注：本接口只对数据类型为 GOLDEN_DATETIME 的标签点有效。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_single_datetime_value(
                              golden_int32 handle,
                              golden_int32 id,
                              golden_int32 datetime, 
                              golden_int16 ms, 
                              const golden_byte *blob,
                              golden_int16 len,
                              golden_int16 quality
                              );



/**
* 命名：goh_put_archived_blob_values
* 功能：写入批量标签点批量字符串型历史存储数据
* 参数：
*        [handle]        连接句柄
*        [count]         整型，输入/输出，
*                        输入时表示 ids、datetimes、ms、lens、blobs、qualities、errors 的长度，
*                        即历史值个数；输出时返回实际写入的数值个数
*        [ids]           整型数组，输入，标签点标识
*        [datetimes]     整型数组，输入，表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示对应的历史数值时间毫秒；否则忽略。
*        [blobs]         字节型指针数组，输入，实时二进制/字符串数值
*        [lens]          短整型数组，输入，二进制/字符串数值长度，
*                        表示对应的 blobs 指针指向的缓冲区长度，超过一个页大小数据将被截断。
*        [qualities]     短整型数组，输入，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]        无符号整型数组，输出，写入历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、lens、blobs、qualities、errors 的长度与 count 一致，
*        本接口仅对数据类型为 GOLDEN_STRING、GOLDEN_BLOB 的标签点有效。
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/

GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_archived_blob_values(
                            golden_int32 handle, 
                            golden_int32 *count, 
                            const golden_int32 *ids,
                            const golden_int32 *datetimes, 
                            const golden_int16 *ms, 
                            const golden_byte* const* blobs, 
                            const golden_int16 *lens, 
                            const golden_int16 *qualities, 
                            golden_error* errors
                            );

/**
* 命名：goh_flush_archived_values
* 功能：将标签点未写满的补历史缓存页写入存档文件中。
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识。
*        [count]         整型，输出，缓存页中数据个数。
* 备注：补历史缓存页写满后会自动写入存档文件中，不满的历史缓存页也会写入文件，
*      但会有一个时间延迟，在此期间此段数据可能查询不到，为了及时看到补历史的结果，
*      应在结束补历史后调用本接口。
*      count 参数可为空指针，对应的信息将不再返回。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_flush_archived_values(
                          golden_int32 handle, 
                          golden_int32 id, 
                          golden_int32 *count
                          );


/**
* 命名：goh_get_single_named_type_value
* 功能：读取单个自定义类型标签点某个时间的历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*        [mode]          整型，输入，取值 GOLDEN_NEXT、GOLDEN_PREVIOUS、GOLDEN_EXACT 之一：
*                        GOLDEN_NEXT 寻找下一个最近的数据；
*                        GOLDEN_PREVIOUS 寻找上一个最近的数据；
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*        [datetime]      整型，输入/输出，输入时表示时间秒数；
*                        输出时表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输入/输出，如果 id 指定的标签点时间精度为毫秒，
*                        则输入时表示时间毫秒数；输出时表示实际取得的历史数值时间毫秒数。
*                        否则忽略输入，输出时为 0。
*        [object]        void数组，输出，自定义类型标签点历史值
*        [length]        短整型，输入/输出，输入时表示 object 的长度，
*                        输出时表示实际获取的自定义类型标签点数据长度。
*        [quality]       短整型，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_get_single_named_type_value(
    golden_int32 handle,
    golden_int32 id, 
    golden_int32 mode, 
    golden_int32 *datetime,
    golden_int16 *ms, 
    void *object,
    golden_int16 *length,
    golden_int16 *quality
    );

/**
* 命名：goh_get_archived_named_type_values
* 功能：连续读取自定义类型标签点的历史数据
* 参数：
*        [handle]        连接句柄
*        [id]            整型，输入，标签点标识
*                        GOLDEN_EXACT 取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
*        [datetime1]     整型，输入，表示开始时间秒数；
*        [ms1]           短整型，输入，指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；
*        [datetime2]     整型，输入,表示结束时间秒数；
*        [ms2]           短整型，输入，指定的标签点时间精度为毫秒，
*                        表示时间毫秒数；
*        [length]        短整型数组，输入，输入时表示 objects 的长度，
*        [count]         整型，输入/输出，输入表示想要查询多少数据
*                        输出表示实际查到多少数据
*        [datetimes]     整型数组，输出，表示实际取得的历史数值对应的时间秒数。
*        [ms]            短整型，输出，如果 id 指定的标签点时间精度为毫秒，
*                        表示实际取得的历史数值时间毫秒数。
*        [objects]       void类型数组，输出，自定义类型标签点历史值
*        [qualities]     短整型数组，输出，历史值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
goh_get_archived_named_type_values(
    golden_int32 handle,
    golden_int32 id,
    golden_int32 datetime1,
    golden_int16 ms1,
    golden_int32 datetime2,
    golden_int16 ms2,
    golden_int16 length,
    golden_int32 *count,
    golden_int32 *datetimes,
    golden_int16 *ms,
    void* const* objects,
    golden_int16 *qualities
    );

/**
* 命名：goh_put_single_named_type_value
* 功能：写入自定义类型标签点的单个历史事件
* 参数：
*        [handle]    连接句柄
*        [id]        整型，输入，标签点标识
*        [datetime]  整型，输入，数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型，输入，历史数值时间，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则忽略
*        [object]    void数组，输入，历史自定义类型标签点数值
*        [length]    短整型，输入，自定义类型标签点数值长度，超过一个页大小数据将被截断。
*        [quality]   短整型，输入，历史数值品质，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_single_named_type_value(
    golden_int32 handle,
    golden_int32 id,
    golden_int32 datetime, 
    golden_int16 ms, 
    const void *object,
    golden_int16 length,
    golden_int16 quality
    );

/**
* 命名：goh_put_archived_named_type_values
* 功能：批量补写自定义类型标签点的历史事件
* 参数：
*        [handle]        连接句柄
*        [count]         整型，输入/输出，
*                        输入时表示 ids、datetimes、ms、lens、blobs、qualities、errors 的长度，
*                        即历史值个数；输出时返回实际写入的数值个数
*        [ids]           整型数组，输入，标签点标识
*        [datetimes]     整型数组，输入，表示对应的历史数值时间秒数。
*        [ms]            短整型数组，输入，如果 id 指定的标签点时间精度为毫秒，
*                        表示对应的历史数值时间毫秒；否则忽略。
*        [objects]       void类型指针数组，输入，自定义类型标签点数值
*        [lengths]       短整型数组，输入，自定义类型标签点数值长度，
*                        表示对应的 objects 指针指向的缓冲区长度，超过一个页大小数据将被截断。
*        [qualities]     短整型数组，输入，历史数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]        无符号整型数组，输出，写入历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、lens、objects、qualities、errors 的长度与 count 一致，
*        如果 datetimes、ms 标识的数据已经存在，其值将被替换。
*/

GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goh_put_archived_named_type_values(
      golden_int32 handle, 
      golden_int32 *count, 
      const golden_int32 *ids,
      const golden_int32 *datetimes, 
      const golden_int16 *ms, 
      const void* const* objects, 
      const golden_int16 *lengths, 
      const golden_int16 *qualities, 
      golden_error* errors
      );

/************************************ 方程式计算接口 ************************************/



/**
* 命名：goe_compute_history
* 功能：重算或补算批量计算标签点历史数据
* 参数：
*        [handle]        连接句柄
*        [count]         整型，输入/输出，
*                        输入时表示 ids、errors 的长度，
*                        即标签点个数；输出时返回成功开始计算的标签点个数
*        [flag]          短整型，输入，不为 0 表示进行重算，删除时间范围内已经存在历史数据；
*                        为 0 表示补算，保留时间范围内已经存在历史数据，覆盖同时刻的计算值。
*        [datetime1]     整型，输入，表示起始时间秒数。
*        [ms1]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示起始时间对应的毫秒；否则忽略
*        [datetime2]     整型，输入，表示结束时间秒数。如果为 0，表示计算直至存档中数据的最后时间
*        [ms2]           短整型，输入，如果 id 指定的标签点时间精度为毫秒，表示结束时间对应的毫秒；否则忽略
*        [ids]           整型数组，输入，标签点标识
*        [errors]        无符号整型数组，输出，计算历史数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、errors 的长度与 count 一致，本接口仅对带有计算扩展属性的标签点有效。
*        由 datetime1、ms1 表示的时间可以大于 datetime2、ms2 表示的时间，
*        此时前者表示结束时间，后者表示起始时间。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goe_compute_history(
                    golden_int32 handle, 
                    golden_int32 *count,
                    golden_int16 flag,
                    golden_int32 datetime1, 
                    golden_int16 ms1, 
                    golden_int32 datetime2, 
                    golden_int16 ms2, 
                    const golden_int32 *ids, 
                    golden_error *errors
                    );

/**
* 命名：goe_get_equation_graph_count
* 功能：根据标签点 id 获取相关联方程式键值对数量
* 参数：
*      [handle]   连接句柄
*      [id]       整型，输入，标签点标识
*      [flag]     枚举，输入，获取的拓扑图的关系
*      [count]    整型，输入，拓扑图键值对数量
* 备注：键值对为数据结构，存储方程式涉及到的各标签点ID、及其父ID等
*		具体参考goe_get_equation_graph_datas
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goe_get_equation_graph_count(
							golden_int32 handle,
							golden_int32 id, 
              GOLDEN_GRAPH_FLAG flag,
							golden_int32 *count
							);

/**
* 命名：goe_get_equation_graph_datas
* 功能：根据标签点 id 获取相关联方程式键值对数据
* 参数：
*      [handle]   连接句柄
*      [id]       整型，输入，标签点标识
*      [flag]     枚举，输入，获取的拓扑图的关系
*      [count]    整型，输出
                    输入时，表示拓扑图键值对数量
                    输出时，表示实际获取到的拓扑图键值对数量
*      [graph]    输出，GOLDE_GRAPH数据结构，拓扑图键值对信息
* 备注：键值对为数据结构，存储方程式涉及到的各标签点ID、及其父ID等
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
goe_get_equation_graph_datas(
								golden_int32 handle,
								golden_int32 id, 
                GOLDEN_GRAPH_FLAG flag,
								golden_int32 *count,
								GOLDEN_GRAPH *graph 
								);


/************************************ 查询标签点属性接口 ************************************/

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_tags_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *tags,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_ms_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *ms,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_types_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *types,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_compdevs_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *compdevs,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_compmaxs_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *compmaxs,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_compmins_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *compmins,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_excdevs_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *excdevs,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_excmaxs_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *excmaxs,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_excmins_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *excmins,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_classofs_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_uint8 *classofs,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_tables_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_uint16 *tables,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_summarys_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *summarys,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_mirrors_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *mirrors,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_compress_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *compress,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_steps_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *steps,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_shutdowns_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *shutdowns,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_archives_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *archives,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_table_dot_tags_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *table_dot_tags,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_descs_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *descs,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_units_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *units,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_changers_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *changers,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_creators_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *creators,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_lowlimits_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *lowlimits,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_highlimits_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *highlimits,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_typicals_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *typicals,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_changedates_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *changedates,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_createdates_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *createdates,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_digits_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int16 *digits,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_compdevpercents_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *compdevpercents,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_excdevpercents_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *excdevpercents,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_sources_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *sources,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_scans_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *scans,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_instruments_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *instruments,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_location1s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *location1s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_location2s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *location2s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_location3s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *location3s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_location4s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *location4s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_location5s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *location5s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_userint1s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *userint1s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_userint2s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *userint2s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_userreal1s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *userreal1s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_userreal2s_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_float32 *userreal2s,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_equations_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  char* const *equations,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_triggers_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *triggers,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_timecopys_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_byte *timecopys,
  golden_error *errors
  );

GOLDENAPI
  golden_error
  GOLDENAPI_CALLRULE 
  gob_get_periods_prpperty(
  golden_int32 handle,
  golden_int32 *count,
  golden_int32 *ids,
  golden_int32 *periods,
  golden_error *errors
  );


/************************************ 查询性能计数信息接口 ************************************/

/**
* 命名：gop_get_perf_tags_count
* 功能：获取Perf服务中支持的性能计数点的数量
* 参数：
*      [handle]   连接句柄
*      [count]    整型，输出，表示实际获取到的Perf服务中支持的性能计数点的数量
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE 
gop_get_perf_tags_count(
  golden_int32 handle,
  int* count);


/**
* 命名：gop_get_perf_tags_info
* 功能：根据性能计数点ID获取相关的性能计数点信息
* 参数：
*      [handle]   连接句柄
*      [count]    整型，输入，输出
                    输入时，表示想要获取的性能计数点信息的数量，也表示tags_info，errors等的长度
                    输出时，表示实际获取到的性能计数点信息的数量
       [errors] 无符号整型数组，输出，获取性能计数点信息的返回值列表，参考golden_error.h
* 备注：用户须保证分配给 tags_info，errors 的空间与 count 相符
*/
GOLDENAPI
golden_error 
  GOLDENAPI_CALLRULE 
  gop_get_perf_tags_info(
  golden_int32 handle, 
  golden_int32* count, 
  GOLDEN_PERF_TAG_INFO* tags_info, 
  golden_error* errors);


/**
* 命名：gop_get_perf_values
* 功能：批量读取性能计数点的当前快照数值
* 参数：
*        [handle]    连接句柄
*        [count]     整型，输入/输出，性能点个数，
*                    输入时表示 perf_ids、datetimes、ms、values、states、qualities、errors 的长度，
*                    输出时表示成功获取实时值的性能计数点个数
*        [perf_ids]  整型数组，输入，性能计数点标识列表，参考GOLDEN_PERF_TAG_ID
*        [datetimes] 整型数组，输出，实时数值时间列表,
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输出，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，返回相应的毫秒值；否则为 0
*        [values]    双精度浮点型数组，输出，实时浮点型数值列表，
*                    对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，返回相应的快照值；否则为 0
*        [states]    64 位整型数组，输出，实时整型数值列表，
*                    对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                    GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，返回相应的快照值；否则为 0
*        [qualities] 短整型数组，输出，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY 
*        [errors]    无符号整型数组，输出，读取实时数据的返回值列表，参考golden_error.h
* 备注：用户须保证 ids、datetimes、ms、values、states、qualities、errors 的长度与 count 一致。
*/
GOLDENAPI
golden_error 
GOLDENAPI_CALLRULE
  gop_get_perf_values(
  golden_int32 handle,
  golden_int32* count, 
  int* perf_ids, 
  int* datetimes, 
  golden_int16* ms, 
  golden_float64* values, 
  golden_int64* states, 
  golden_int16* qualities, 
  golden_error* errors);


#pragma region 转发服务配置模块通信接口 add by yuanbo.sheng 2015.04.01
/**
* 命名：god_get_plugin_info_total_size
* 功能：获取所有转发插件信息总长度
* 参数：
*        [handle]    连接句柄
*        [total_plugin_size]  插件信息总长度，输出；
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_get_plugin_info_total_size(golden_int32 handle, golden_int32& total_plugin_size);

/**
* 命名：god_get_plugin_info_array
* 功能：获取所有转发插件信息
* 参数：
*        [handle]    连接句柄
*        [plugin_info_array] 字符串缓存区，长度需等于total_plugin_size；
*        [total_plugin_size]  插件信息总长度，输出；
 */
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_get_plugin_info_array(golden_int32 handle, char* plugin_info_array, golden_int32& total_plugin_size);

/**
* 命名：god_register_plugin
* 功能：注册插件信息
* 参数：
*        [handle]    连接句柄
*        [_plugin_info] 字符串缓存区，长度需等于plugin_size；
*        [_plugin_size]  插件信息长度；
*        [plugin_id] 插件编号，输出；
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_register_plugin(golden_int32 handle, const char* _plugin_info, golden_int32 plugin_size, golden_int32& plugin_id);

/**
* 命名：god_unregister_plugin
* 功能：卸载插件信息
* 参数：
*        [handle]    连接句柄
*        [plugin_id] 插件编号；
 */
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_unregister_plugin(golden_int32 handle, golden_int32 plugin_id);

/*
 *
 * 按插件编号启动插件
 *
 * inputs:
 *     golden_int32 handle: 网络通信句柄
 *     golden_int32 plugin_id: 插件编号
 * returns:
 *     0 成功  其他 失败
 */
 
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
  god_start_configed_plugin_by_id(
  golden_int32 handle,
  golden_int32 plugin_id);

/*
 *
 * 启动转发服务器所有已配置的插件
 * 
 *  inputs:
 *      golden_int32 handle: 网络通信句柄 
 *  return:
 *      0 成功  其他 失败
 */

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
  god_start_all_configed_plugins(
  golden_int32 handle);

/*
*
* 按插件编号启动插件
*
* inputs:
*     golden_int32 handle: 网络通信句柄
*     golden_int32 plugin_id: 插件编号
* returns:
*     0 成功  其他 失败
*/

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
  god_stop_configed_plugin_by_id(
  golden_int32 handle,
  golden_int32 plugin_id);

/*
 * 
 *  停止运行所有已配置的插件
 *  
 *  inputs:
 *      golden_int32 handle: 网络通信句柄
 *  return:
 *      0 成功  其他 失败
 */
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
  god_stop_all_configed_plugins(
  golden_int32 handle);

/*
 *
 * 按插件编号设置插件的启用状态
 *
 * inputs:
 *     golden_int32 handle: 网络通信句柄
 *     golden_int8  enable_status: 插件启用状态 具体值见插件信息结构体golden_output_plugin_info_t
 *     golden_int32 plugin_id: 插件编号
 *     
 * returns:
 *     0 成功  其他 失败
 */

GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
  god_switch_plugin_load_state(
  golden_int32 handle,
  golden_int32 plugin_id,
  golden_int32  enable_status);

/*
* 根据插件编号读取指定插件信息数据长度
*
* @input:
golden_int32 handle: 数据库连接句柄
golden_int32 plugin_id: 插件编号
@output:
golden_int32 plugin_info_size：插件信息数据长度
@return:
0 成功 其它 失败
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_get_single_plugin_info_data_size (
golden_int32 handle,
golden_int32 plugin_id,
golden_int32& plugin_info_size);

/*
* 根据插件编号读取指定插件信息数据
*
* @input:
    golden_int32 handle: 数据库连接句柄
    golden_int32 plugin_id: 插件编号
  @output:
    char* plugin_info_data: 插件信息数据
    golden_int32 plugin_info_size：插件信息数据长度
  @return:
    0 成功 其它 失败
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_get_single_plugin_info_data (
  golden_int32 handle,
  golden_int32 plugin_id,
  char* plugin_info_data,
  golden_int32& plugin_info_size
  );

/*
*  根据插件编号更新插件信息
   @input:
    golden_int32 handle: 数据库连接句柄
    golden_int32 plugin_id: 插件编号
    const char* plugin_info_data : 配置数据
    golden_int32 plugin_info_size : 配置内容长度
   @output:
     无
   @return:
     0 成功 其它 失败
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_set_single_plugin_info_data (
golden_int32 handle,
golden_int32 plugin_id,
const char* plugin_info_data,
golden_int32 plugin_info_size
);

/*
* 根据插件编号读取指定插件的配置文件内容长度
*
* @input:
golden_int32 handle: 数据库连接句柄
golden_int32 plugin_id: 插件编号
@output:
golden_int32 config_dize：配置数据长度
@return:
0 成功 其它 失败
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_get_download_plugin_config_data_size(
golden_int32 handle,
golden_int32 plugin_id,
golden_int32& config_size);

/*
* 根据插件编号读取指定插件的配置文件内容
*
* @input:
    golden_int32 handle: 数据库连接句柄
    golden_int32 plugin_id: 插件编号
  @output:
    char* config_data: 配置数据
    golden_int32 config_dize：配置数据长度
  @return:
    0 成功 其它 失败
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_download_plugin_config_data_by_id(
  golden_int32 handle,
  golden_int32 plugin_id,
  char* config_data,
  golden_int32& config_size
  );

/*
*  根据插件编号更新插件的配置文件内容

   @input:
    golden_int32 handle: 数据库连接句柄
    golden_int32 plugin_id: 插件编号
    const char* config_data : 配置数据
    golden_int32 config_size : 配置内容长度
   @output:
     无
   @return:
     0 成功 其它 失败
*/
GOLDENAPI
golden_error
GOLDENAPI_CALLRULE
god_upload_plugin_config_data_by_id(
  golden_int32 handle,
golden_int32 plugin_id,
const char* config_data,
golden_int32 config_size
);

#pragma endregion

#endif  // __GOLDEN_API_H__