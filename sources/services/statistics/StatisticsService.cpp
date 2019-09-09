#include "StatisticsService.h"

static std::string prefix = "<!DOCTYPE html>\n"
                            "<html lang=\"en\">\n"
                            "<head>\n"
                            "    <meta charset=\"UTF-8\">\n"
                            "    <title>onyxup status page</title>\n"
                            "    <h3>Статистика работы сервера</h3>\n"
                            "<style>\n"
                            "table {\n"
                            "border-spacing: 10px;\n"
                            "color: white;\n"
                            "}\n"
                            "</style>\n"
                            "</head>\n"
                            "<body>\n";
static std::string suffix = "</body>\n"
                            "</html>";

onyxup::ResponseBase onyxup::StatisticsService::callback(PtrCRequest request) {
    std::ostringstream os;
    computeCurrentNumberReadRequests();
    computeCurrentNumberWriteRequests();
    computeCurrentNumberWaitRequests();
    os << prefix
            << "<hr/>\n"
            << "<p>\n"
            << "<b>Число активных клиентских соединений = " << 1 << "</b>\n"
            << "</p>\n"
            << "<hr/>\n"
            << "<p>\n"
            << "<table width=\"500px\" bgcolor=\"#FFA500\">\n"
            << "<tr>\n"
            << "<td><b>Общее количество принятых подключений</b></td>\n"
            << "<td><b>" << totalNumberConnectionsAccepted << "</b></td>\n"
            << "</tr>\n"
            << "<tr>\n"
            << "<td><b>Общее количество обработанных подключений</b></td>\n"
            << "<td><b>" << totalNumberConnectionsProcessed << "</b></td>\n"
            << "</tr>\n"
            << "<tr>\n"
            << "<td><b>Суммарное число клиентских запросов</b></td>\n"
            << "<td><b>" << totalNumberClientRequests << "</b></td>\n"
            << "</tr>\n"
            << "</p>\n"
            << "<p>\n"
            << "<table width=\"500px\" bgcolor=\"#32CD32\">\n"
            << "<tr>\n"
            << "<td><b>Текущее число запросов чтения</b></td>\n"
            << "<td><b>" << currentNumberReadRequests << "</b></td>\n"
            << "</tr>\n"
            << "<tr>\n"
            << "<td><b>Текущее число запросов записи</b></td>\n"
            << "<td><b>" << currentNumberWriteRequests << "</b></td>\n"
            << "</tr>\n"
            << "<tr>\n"
            << "<td><b>Текущее число открытых бездействующих соединений</b></td>\n"
            << "<td><b>" << currentNumberWaitRequests << "</b></td>\n"
            << "</tr>\n"
            << "<tr>\n"
            << "<td><b>Текущее число задач в очереди воркеров</b></td>\n"
            << "<td><b>" << currentNumberTasks << "</b></td>\n"
            << "</tr>\n"
            << "</p>\n"
            << suffix;

    return ResponseBase(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG,
                        MimeType::MIME_TYPE_TEXT_HTML, os.str());
}


void onyxup::StatisticsService::addTotalNumberConnectionsAccepted() {
    totalNumberConnectionsAccepted++;
}

void onyxup::StatisticsService::addTotalNumberConnectionsProcessed() {
    totalNumberConnectionsProcessed++;
}

void onyxup::StatisticsService::addTotalNumberClientRequests() {
    totalNumberClientRequests++;
}

onyxup::StatisticsService::StatisticsService(const PtrBuffer *buffers, const PtrRequest *requests, size_t n) {
    buffersAndRequestsLength = n;
    this->buffers = buffers;
    this->requests = requests;
    totalNumberConnectionsAccepted = 0;
    totalNumberConnectionsProcessed = 0;
    totalNumberClientRequests = 0;
    currentNumberReadRequests = 0;
    currentNumberWriteRequests = 0;
    currentNumberWaitRequests = 0;
    currentNumberTasks = 0;
}

void onyxup::StatisticsService::computeCurrentNumberReadRequests() {
    currentNumberReadRequests = 0;
    for (size_t i = 0; i < buffersAndRequestsLength; i++) {
        if (requests[i] && buffers[i]) {
            if (!requests[i]->isHeaderAccept() && buffers[i]->getPosInputBuffer() > 0)
                currentNumberReadRequests++;
        }
    }
}

void onyxup::StatisticsService::computeCurrentNumberWriteRequests() {
    currentNumberWriteRequests = 0;
    for (size_t i = 0; i < buffersAndRequestsLength; i++) {
        if (requests[i] && buffers[i]) {
            if (requests[i]->isHeaderAccept() && buffers[i]->getPosOutputBuffer() > 0)
                currentNumberWriteRequests++;
        }
    }
}

void onyxup::StatisticsService::computeCurrentNumberWaitRequests() {
    currentNumberWaitRequests = 0;
    for (size_t i = 0; i < buffersAndRequestsLength; i++) {
        if (requests[i])
            currentNumberWaitRequests++;
    }
    currentNumberWaitRequests =
            currentNumberWaitRequests - currentNumberReadRequests - currentNumberWriteRequests;
}

void onyxup::StatisticsService::setCurrentNumberTasks(int number) {
    currentNumberTasks = number;
}