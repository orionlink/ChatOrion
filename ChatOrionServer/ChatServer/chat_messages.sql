SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

CREATE TABLE IF NOT EXISTS chat_messages (
           id BIGINT PRIMARY KEY AUTO_INCREMENT,
           msg_id VARCHAR(64) NOT NULL COMMENT '消息唯一ID',
           from_uid INT NOT NULL COMMENT '发送者ID',
           to_uid INT NOT NULL COMMENT '接收者ID',
           content TEXT NOT NULL COMMENT '消息内容',
           msg_type TINYINT NOT NULL DEFAULT 1 COMMENT '消息类型：1文本，2图片，3语音...',
           send_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '发送时间',
           status TINYINT NOT NULL DEFAULT 0 COMMENT '消息状态：0未读，1已读，2已删除',
           INDEX idx_msg_id (msg_id),
           INDEX idx_from_uid (from_uid),
           INDEX idx_to_uid (to_uid),
           INDEX idx_send_time (send_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic COMMENT='聊天消息表';

CREATE TABLE IF NOT EXISTS chat_message_relation (
           id BIGINT PRIMARY KEY AUTO_INCREMENT,
           user_id INT NOT NULL COMMENT '用户ID',
           peer_id INT NOT NULL COMMENT '对方ID',
           last_msg_id VARCHAR(64) NOT NULL COMMENT '最后一条消息ID',
           unread_count INT NOT NULL DEFAULT 0 COMMENT '未读消息数',
           last_update_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
           INDEX idx_user_peer (user_id, peer_id),
           INDEX idx_last_update (last_update_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic COMMENT='消息关系表';