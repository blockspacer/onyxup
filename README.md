## onyxup
Микро фреймворк для разработки веб приложений

[![Build Status](https://travis-ci.org/myduomilia/onyxup.svg?branch=master)](https://travis-ci.org/myduomilia/onyxup)

## Используемые сторонние библиотеки
[Plog - portable, simple and extensible C++ logging library](https://github.com/SergiusTheBest/plog/tree/master/include/plog)

[PicoHTTPParser - tiny HTTP parser](https://github.com/h2o/picohttpparser)

[Gzip C++ lib for gzip compression and decompression](https://github.com/mapbox/gzip-hpp)

[JSON for modern C++](https://github.com/nlohmann/json)

## Зависимости
```bash
sudo apt-get install zlib1g-dev
sudo apt-get install libcurl3-dev (only for tests)

```


## Установка стабильной версии:
```bash
wget -O onyxup-1.1.0.tar.gz  https://codeload.github.com/myduomilia/onyxup/tar.gz/1.1.0
tar -zxvf onyxup-1.1.0.tar.gz 
cd onyxup-1.1.0
mkdir build
cd build/
cmake ..
make
sudo make install
```

## Установка последней версии:
```bash
mkdir build
cd build/
cmake ..
make
sudo make install
```

## Установка и запуск тестов:
```bash
mkdir build
cd build/
cmake -DBUILD_TESTING=on  ..
make
make test
sudo make install
```

## Установка в режиме debug:
```bash
mkdir build
cd build/
cmake -DBUILD_DEBUG_MODE=on  ..
make
make test
sudo make install
```

## Пример использования:

```C++
#include <onyxup/server/server.h>
#include <onyxup/server/utils.h>
#include <onyxup/response/response-json.h>
#include <onyxup/response/response-html.h>
#include <onyxup/response/response-file.h>
#include <string>

onyxup::ResponseBase html(onyxup::PtrCRequest request);

onyxup::ResponseBase json(onyxup::PtrCRequest request);

onyxup::ResponseBase file(onyxup::PtrCRequest request);

onyxup::ResponseBase params(onyxup::PtrCRequest request);

onyxup::ResponseBase multipartForm(onyxup::PtrCRequest request);

int main() {

    onyxup::HttpServer server(7000, 16);

    /*
     * Routes
     */
    server.addRoute("GET", "^/html$", html, onyxup::EnumTaskType ::LOCAL_TASK);
    server.addRoute("GET", "^/json", json, onyxup::EnumTaskType ::LOCAL_TASK);
    server.addRoute("GET", "^/file$", file,onyxup::EnumTaskType ::LOCAL_TASK);
    server.addRoute("GET", "^/params.+$", params,onyxup::EnumTaskType ::LOCAL_TASK);
    server.addRoute("POST", "^/multipart-form", multipart_form, onyxup::EnumTaskType ::LOCAL_TASK);
    /*
     * Route для статических файлов
     */
    server.addRoute("GET", "^/static/.+$", onyxup::HttpServer::defaultCallbackStaticResources, onyxup::EnumTaskType ::STATIC_RESOURCES_TASK);

    /*
     * Путь до расположения статических файлов
     */
    std::string pathToStaticFiles = "/project/";
    onyxup::HttpServer::setPathToStaticResources(pathToStaticFiles);

    /*
    *   Максимальное время выполнения запроса на сервер (по умолчанию 60 с)
    */
    onyxup::HttpServer::setTimeLimitRequestSeconds(30);

    /*
    *   Масимальное количество задач в очереди на сервере (по умолчанию 100)
    */
    onyxup::HttpServer::setLimitLocalTasks(100);

    /*
     * Включаем сбор статистики и указываем url доступа (по умолчанию "^/onyxup-status-page$")
     */
    onyxup::HttpServer::setStatisticsEnable(true);
    onyxup::HttpServer::setStatisticsUrl("^/statistics$");

    server.run();
}

onyxup::ResponseBase html(onyxup::PtrCRequest request) {
    return onyxup::ResponseHtml("index.html");
}

onyxup::ResponseBase json(onyxup::PtrCRequest request) {
    return onyxup::ResponseJson(R"({"status":"success"})", true);
}

onyxup::ResponseBase file(onyxup::PtrCRequest request) {
    std::string pathToFile;
    return onyxup::ResponseFile(pathToFile);
}

onyxup::ResponseBase params(onyxup::PtrCRequest request) {
    /*
     * Запрос /params?param-first=1&param-second=data
     */
    auto & params = request->getParams();
    if(params.find("param-first") != params.end()){
        int param1 =  std::stoi(params.at("param-first"), nullptr);
        std::cout << "Param first = " << param1 << std::endl;
    }
    if(params.find("param-second") != params.end()){
        std::string param1 =  params.at("param-second");
        std::cout << "Param second = " << param1 << std::endl;
    }
    return onyxup::ResponseJson(R"({"status":"success"})");
}

onyxup::ResponseBase multipartForm(onyxup::PtrCRequest request) {
    auto fields = onyxup::utils::multipartFormData(request);
    std::vector<char> data = fields["image"].getData();
    std::string filename = "./" + fields["image"].getFilename();
    std::ofstream f(filename, std::ios::binary);
    std::copy(data.begin(), data.end(), std::ostream_iterator<char>(f, ""));
    return onyxup::ResponseJson(R"({"status":"success"})", true);
}
```

## Лицензия

MIT License

