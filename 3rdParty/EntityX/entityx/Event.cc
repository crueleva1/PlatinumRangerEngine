/*
 * Copyright (C) 2012 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */
#include "EntityXPCH.h"
#include "entityx/Event.h"

namespace entityx {

ENTITYX_API BaseEvent::Family BaseEvent::family_counter_ = 0;

BaseEvent::~BaseEvent() {
}

EventManager::EventManager() {
}

EventManager::~EventManager() {
}

}  // namespace entityx
