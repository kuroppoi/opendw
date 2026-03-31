#ifndef __EVENT_LISTENER_CONTAINER_H__
#define __EVENT_LISTENER_CONTAINER_H__

#include "axmol.h"

namespace opendw
{

/*
 * Managing custom event listeners is too verbose, so this class offers a simple solution for that.
 * All you need to do is inherit from it, call `addEventListener` to register custom event listeners
 * and call `removeEventListeners` when you no longer need them to easily remove all of them.
 */
class EventListenerContainer
{
public:
    typedef std::function<void(ax::EventCustom*)> EventCallback;

    void addEventListener(std::string_view eventName, const EventCallback& callback);
    void removeEventListeners();

protected:
    std::vector<ax::EventListenerCustom*> _eventListeners;
};

}  // namespace opendw

#endif  // __EVENT_LISTENER_CONTAINER_H__
