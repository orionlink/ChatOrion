const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module =require('./const')
const uuidv4 = require('uuid').v4
const emailModule = require('./email')
const redis_module = require('./redis')

async function GetVarifyCode(call, callback) {
    console.log("email is ", call.request.email);
    try {
        // 从 Redis 中查询验证码
        let query_res = await redis_module.GetRedis(const_module.code_prefix + call.request.email);
        console.log("query_res is ", query_res);

        let uniqueId = query_res;
        if (query_res == null) {
            // 生成唯一的验证码
            uniqueId = uuidv4();
            if (uniqueId.length > 4) {
                uniqueId = uniqueId.substring(0, 4); // 截取前 4 位
            }

            // 将验证码存入 Redis，设置过期时间为 600 秒
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix + call.request.email, uniqueId, 600);
            if (!bres) {
                callback(null, {
                    email: call.request.email,
                    error: const_module.Errors.RedisErr
                });
                return;
            }
        }

        console.log("uniqueId is ", uniqueId);

        // 构造邮件内容
        let text_str = '您的验证码为' + uniqueId + '，请三分钟内完成注册';

        // 发送邮件
        let mailOptions = {
            from: 'orionlink@163.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res);

        // 返回成功响应
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success
        });
    } catch (error) {
        console.log("catch error is ", error);
        // 返回异常响应
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}
function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VarifyService.service, { GetVarifyCode: GetVarifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')
    })
}
main()