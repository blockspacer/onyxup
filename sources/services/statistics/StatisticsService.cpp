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
       << "<td><b>" << m_total_number_connections_accepted << "</b></td>\n"
       << "</tr>\n"
       << "<tr>\n"
       << "<td><b>Общее количество обработанных подключений</b></td>\n"
       << "<td><b>" << m_total_number_connections_processed << "</b></td>\n"
       << "</tr>\n"
       << "<tr>\n"
       << "<td><b>Суммарное число клиентских запросов</b></td>\n"
       << "<td><b>" << m_total_number_client_requests << "</b></td>\n"
       << "</tr>\n"
       << "</p>\n"
       << "<p>\n"
       << "<table width=\"500px\" bgcolor=\"#32CD32\">\n"
       << "<tr>\n"
       << "<td><b>Текущее число запросов чтения</b></td>\n"
       << "<td><b>" << m_current_number_read_requests << "</b></td>\n"
       << "</tr>\n"
       << "<tr>\n"
       << "<td><b>Текущее число запросов записи</b></td>\n"
       << "<td><b>" << m_current_number_write_requests << "</b></td>\n"
       << "</tr>\n"
       << "<tr>\n"
       << "<td><b>Текущее число открытых бездействующих соединений</b></td>\n"
       << "<td><b>" << m_current_number_wait_requests << "</b></td>\n"
       << "</tr>\n"
       << "<tr>\n"
       << "<td><b>Текущее число задач в очереди воркеров</b></td>\n"
       << "<td><b>" << m_current_number_tasks << "</b></td>\n"
       << "</tr>\n"
       << "</p>\n"
       << suffix;

    return ResponseBase(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG,
                        MimeType::MIME_TYPE_TEXT_HTML, os.str());
}


void onyxup::StatisticsService::addTotalNumberConnectionsAccepted() {
    m_total_number_connections_accepted++;
}

void onyxup::StatisticsService::addTotalNumberConnectionsProcessed() {
    m_total_number_connections_processed++;
}

void onyxup::StatisticsService::addTotalNumberClientRequests() {
    m_total_number_client_requests++;
}

onyxup::StatisticsService::StatisticsService(const PtrBuffer *buffers, const PtrRequest *requests, size_t length) {
    m_length_buffers_and_requests = length;
    m_buffers = buffers;
    m_requests = requests;
    m_total_number_connections_accepted = 0;
    m_total_number_connections_processed = 0;
    m_total_number_client_requests = 0;
    m_current_number_read_requests = 0;
    m_current_number_write_requests = 0;
    m_current_number_wait_requests = 0;
    m_current_number_tasks = 0;
}

void onyxup::StatisticsService::computeCurrentNumberReadRequests() {
    m_current_number_read_requests = 0;
    for (size_t i = 0; i < m_length_buffers_and_requests; i++) {
        if (m_requests[i] && m_buffers[i]) {
            if (!m_requests[i]->isHeaderAccept() && m_buffers[i]->getInBufferPosition() > 0)
                m_current_number_read_requests++;
        }
    }
}

void onyxup::StatisticsService::computeCurrentNumberWriteRequests() {
    m_current_number_write_requests = 0;
    for (size_t i = 0; i < m_length_buffers_and_requests; i++) {
        if (m_requests[i] && m_buffers[i]) {
            if (m_requests[i]->isHeaderAccept() && m_buffers[i]->getOutBufferPosition() > 0)
                m_current_number_write_requests++;
        }
    }
}

void onyxup::StatisticsService::computeCurrentNumberWaitRequests() {
    m_current_number_wait_requests = 0;
    for (size_t i = 0; i < m_length_buffers_and_requests; i++) {
        if (m_requests[i])
            m_current_number_wait_requests++;
    }
    m_current_number_wait_requests =
            m_current_number_wait_requests - m_current_number_read_requests - m_current_number_write_requests;
}

void onyxup::StatisticsService::setCurrentNumberTasks(int number) {
    m_current_number_tasks = number;
}