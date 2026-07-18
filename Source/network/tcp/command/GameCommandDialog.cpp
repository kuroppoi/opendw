#include "GameCommandDialog.h"

#include "network/tcp/MessageIdent.h"
#include "util/ArrayUtil.h"
#include "GameManager.h"

USING_NS_AX;

namespace opendw
{

void GameCommandDialog::run()
{
    // TODO: implement dialogs
    auto game = GameManager::getInstance();
    Value alert(std::format("[GameCommandDialog] Dialog data: {}", _data[1].getDescription()));
    game->notify(NotificationType::ALERT, alert);
    game->sendMessage(MessageIdent::DIALOG, _data[0], array_util::arrayOf("cancel"));  // Auto-cancel for now
}

}  // namespace opendw
