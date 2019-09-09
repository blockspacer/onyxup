#pragma once

#include <sstream>

#include "../../response/response-html.h"
#include "../../response/response-base.h"
#include "../../response/response-states.h"
#include "../../mime/types.h"
#include "../../buffer/buffer.h"
#include "../../request/request.h"


namespace onyxup {
    class StatisticsService {
    private:
        unsigned long long totalNumberConnectionsAccepted;
        unsigned long long totalNumberConnectionsProcessed;
        unsigned long long totalNumberClientRequests;

        unsigned long long currentNumberReadRequests;
        unsigned long long currentNumberWriteRequests;
        unsigned long long currentNumberWaitRequests;

        unsigned long long currentNumberTasks;

        const PtrBuffer * buffers;
        const PtrRequest * requests;
        size_t buffersAndRequestsLength;

    public:
        onyxup::ResponseBase callback(PtrCRequest);

        StatisticsService(const PtrBuffer * buffers, const PtrRequest * requests, size_t n);

        void addTotalNumberConnectionsAccepted();
        void addTotalNumberConnectionsProcessed();
        void addTotalNumberClientRequests();

        void setCurrentNumberTasks(int number);

        void computeCurrentNumberReadRequests();
        void computeCurrentNumberWriteRequests();
        void computeCurrentNumberWaitRequests();
    };
}


