#include "task.h"

onyxup::PtrTask onyxup::factoryTask() {
    PtrTask task = new (std::nothrow) Task;
    if (task) {
        task->setRequest(nullptr);
        return task;
    }
    return nullptr;
}