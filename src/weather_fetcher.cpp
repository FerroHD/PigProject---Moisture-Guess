/*
  Copyright (c) 2017-2018 Alessandro Ferrara and Andrea Taglia

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <atomic>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "weather_fetcher.h"

using namespace Pistache;
using namespace Pistache::Http;



WeatherFetcher::WeatherFetcher(){
    std::cout << "Weather fetcher ready" << std::endl;
    auto opts = Http::Client::options()
        .threads(1)
        .maxConnectionsPerHost(8);
    client.init(opts);
}


WeatherFetcher::~WeatherFetcher(){
    // std::cout << "destroying weather_fetcher..." << std::endl;
    client.shutdown();
}

void WeatherFetcher::fetchCurrentWeather(std::string &s){
    std::cout << "Making request..." << std::endl;
    std::string page = "http://api.openweathermap.org/data/2.5/weather?q=MIlan,it&mode=json&appid=013b5d74095d80de286d5f0d93a85fe5";
    int count = 1;

    std::vector<Async::Promise<Http::Response>> responses;

    std::atomic<size_t> completedRequests(0);
    std::atomic<size_t> failedRequests(0);

    auto start = std::chrono::system_clock::now();

    for (int i = 0; i < count; ++i) {
        auto resp = client.get(page).cookie(Http::Cookie("FOO", "bar")).send();
        resp.then([&](Http::Response response) {
            ++completedRequests;
            // std::cout << "Response code = " << response.code() << std::endl;
            auto body = response.body();
            s = body;
            // if (!body.empty())
                // std::cout << "Response body = " << body << std::endl;
        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
    }

    auto sync = Async::whenAll(responses.begin(), responses.end());
    Async::Barrier<std::vector<Http::Response>> barrier(sync);

    barrier.wait_for(std::chrono::seconds(5));

    auto end = std::chrono::system_clock::now();
    // std::cout << "Summary of execution" << std::endl
    //           << "Total number of requests sent     : " << count << std::endl
    //           << "Total number of responses received: " << completedRequests.load() << std::endl
    //           << "Total number of requests failed   : " << failedRequests.load() << std::endl
    //           << "Total time of execution           : "
    //           << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

}