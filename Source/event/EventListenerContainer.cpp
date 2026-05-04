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

void EventListenerContainer::addEventListener(EventListener* listener, Node* node)
{
    if (listener)
    {
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);
        _eventListeners.push_back(listener);
    }
}

void EventListenerContainer::addEventListener(EventListener* listener, int fixedPriority)
{
    if (listener)
    {
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, fixedPriority);
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
