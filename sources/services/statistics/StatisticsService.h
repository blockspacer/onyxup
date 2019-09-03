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
        unsigned long long m_total_number_connections_accepted;
        unsigned long long m_total_number_connections_processed;
        unsigned long long m_total_number_client_requests;

        unsigned long long m_current_number_read_requests;
        unsigned long long m_current_number_write_requests;
        unsigned long long m_current_number_wait_requests;

        unsigned long long m_current_number_tasks;

        const PtrBuffer * m_buffers;
        const PtrRequest * m_requests;
        size_t m_length_buffers_and_requests;

    public:
        onyxup::ResponseBase callback(PtrCRequest);

        StatisticsService(const PtrBuffer * buffers, const PtrRequest * requests, size_t length);

        void addTotalNumberConnectionsAccepted();
        void addTotalNumberConnectionsProcessed();
        void addTotalNumberClientRequests();

        void setCurrentNumberTasks(int number);

        void computeCurrentNumberReadRequests();
        void computeCurrentNumberWriteRequests();
        void computeCurrentNumberWaitRequests();
    };
}


