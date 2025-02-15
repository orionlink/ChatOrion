#include "user_data.h"
#include "common_utils.h"

#include <QDir>

SearchInfo::SearchInfo(int uid, QString name,
    QString nick, QString desc, int sex, QString icon):_uid(uid)
  ,_name(name), _nick(nick),_desc(desc),_sex(sex),_icon(icon){
}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc,
                               QString icon, QString nick, int sex)
    :_from_uid(from_uid),_name(name),
      _desc(desc),_icon(icon),_nick(nick),_sex(sex)
{

}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData> > text_vec)
{
    for(const auto & text: text_vec){
      _chat_msgs.push_back(text);
    }
}

bool EmojiManager::loadEmojiData(const QString &fileName)
{
    QString currentDir = QDir::currentPath();
    QString filePath = QDir(currentDir).filePath(fileName);
    JsonHandler jsonHandler;
    jsonHandler.readJsonFile(filePath);
    m_emojiObject = jsonHandler.getJsonObject();
    return !m_emojiObject.isEmpty();
}
