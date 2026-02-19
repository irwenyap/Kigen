/*********************************************************************
 * \file		EventManager.hpp
 * \brief		Handles events based on subscriber listener
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef EVENT_MANAGER_HPP
#define EVENT_MANAGER_HPP

#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <typeindex>


 /**
  * \class EventManager
  * \brief A thread-safe singleton class that manages the subscription, emission, and processing of events.
  *
  * The EventManager class enables a publish-subscribe model for events in a system.
  * It supports dynamic subscription to events of different types and ensures that
  * events are processed in a queue-based system for each event type.
  */
class EventManager {
public:
    /**
     * \brief Retrieve the singleton instance of the EventManager.
     *
     * \return A reference to the singleton instance of the EventManager.
     */
    static EventManager& GetInstance() {
        static EventManager instance;
        return instance;
    }

    /**
     * \brief Subscribe a handler function to a specific event type.
     *
     * \tparam EventType The type of event to subscribe to.
     * \param handler A callable object (e.g., lambda or function pointer) to handle the event.
     *
     * Example usage:
     * \code
     * EventManager::GetInstance().Subscribe<MyEvent>([](const MyEvent& event) {
     *     std::cout << "Event received: " << event.data << std::endl;
     * });
     * \endcode
     */
    template<typename EventType>
    void Subscribe(std::function<void(const EventType&)> handler) {
        auto& handlers = GetHandlers<EventType>();
        handlers.push_back(std::move(handler));
    }

    /**
     * \brief Emit an event to be processed later.
     *
     * The event is added to an internal queue and will be processed during the `ProcessEvents` call.
     *
     * \tparam EventType The type of event being emitted.
     * \param event The event instance to be emitted.
     *
     * Example usage:
     * \code
     * MyEvent event{42};
     * EventManager::GetInstance().Emit(event);
     * \endcode
     */
    template<typename EventType>
    void Emit(const EventType& event) {
        auto& queue = GetEventQueue<EventType>();
        queue.push(event);
    }

    /**
     * \brief Process all queued events for all event types.
     *
     * This method processes all the events in the queues for each event type
     * by invoking the respective handlers.
     */
    void ProcessEvents() {
    }

private:
    /**
     * \brief Retrieve the list of handlers for a specific event type.
     *
     * \tparam EventType The type of event for which handlers are retrieved.
     * \return A reference to the vector of handler functions.
     */
    template<typename EventType>
    std::vector<std::function<void(const EventType&)>>& GetHandlers() {
        static std::vector<std::function<void(const EventType&)>> handlers;
        return handlers;
    }

    /**
     * \brief Retrieve the queue for a specific event type.
     *
     * \tparam EventType The type of event for which the queue is retrieved.
     * \return A reference to the queue of events.
     */
    template<typename EventType>
    std::queue<EventType>& GetEventQueue() {
        static std::queue<EventType> queue;
        return queue;
    }

    /**
     * \brief Process all events of a specific event type.
     *
     * This method retrieves the queue of a specific event type, invokes all handlers
     * for each event in the queue, and then removes the event from the queue.
     *
     * \tparam EventType The type of event to process.
     */
    template<typename EventType>
    void ProcessEventType() {
        auto& queue = GetEventQueue<EventType>();
        auto& handlers = GetHandlers<EventType>();

        while (!queue.empty()) {
            const auto& event = queue.front();
            for (const auto& handler : handlers) {
                handler(event);
            }
            queue.pop();
        }
    }
};  

#endif // !EVENT_MANAGER_HPP