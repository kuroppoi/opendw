#include "EventListenerContainer.h"

USING_NS_AX;

namespace opendw
{

void EventListenerContainer::addEventListener(std::string_view eventName, const EventCallback& callback)
{
    auto listener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(eventName, callback);

    if (listener)
    {
        _eventListeners.push_back(listener);
    }
}

void EventListenerContainer::removeEventListeners()
{
    for (auto&& listener : _eventListeners)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }

    _eventListeners.clear();
}

}  // namespace opendw
