/*
 * Copyright ©2025 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2025 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

#define BUF_SIZE 4096

using std::map;
using std::string;
using std::vector;

namespace hw4 {

static const char *kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest *const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Hint: Try and read in a large amount of bytes each time you call
  // WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:

  // for starting the while loop
  size_t find_end = buffer_.find(kHeaderEnd);

  // check if we have read \r\n\r\n
  while (find_end == std::string::npos) {
    // a temprorary buffer because we can't pass in buffer_
    unsigned char temp_buf[BUF_SIZE];
    // read the reciving buffer as much as we cna
    int result = WrappedRead(fd_, temp_buf, BUF_SIZE);
    // this means either the user closed the connection, or his connection is
    // dropped by some error
    if (result <= 0) {
      return false;
    }

    // append newly read buf to the buffer
    buffer_.append(string(reinterpret_cast<char *>(temp_buf), result));

    // update the while condition, which is have we meet the end of a request
    find_end = buffer_.find(kHeaderEnd);
  }
  // the request is from the beginning of the buffer_ to the \r\n\r\n
  string req = buffer_.substr(0, find_end + kHeaderEndLen);
  // parse the request
  *request = ParseRequest(req);

  // clean the buffer_ but keep everything after the \r\n\r\n
  buffer_ = buffer_.substr(find_end + kHeaderEndLen);

  return true;
}

bool HttpConnection::WriteResponse(const HttpResponse &response) const {
  // We use a reinterpret_cast<> to cast between unrelated pointer types, and
  // a static_cast<> to perform a conversion from an unsigned type to its
  // corresponding signed one.
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());

  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string &request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Hint: Take a look at HttpRequest.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:
  // split the entire request to lines and store them in the vector
  vector<string> lines;
  boost::split(lines, request, boost::is_any_of("\r\n"));

  // got some lines to process
  if (!lines.empty()){
    // split the first line to parts by white space
    vector<string> first_line;
    boost::split(first_line, lines[0], boost::is_any_of(" "));

    // the second part is the URI([METHOD] [request-uri] HTTP/[version]\r\n)
    if (first_line.size() >= 2) {
      req.set_uri(first_line[1]);
    }

    // parse headers
    for (size_t i = 1; i < lines.size(); i++) {
      // process lines line by line
      string other_line = lines[i];
      boost::trim_right(other_line);

      if (other_line.empty()) {
        // have to use continue, not break, IDK why
        // !!!!!!!!!
        // if we meet \r\n\r\n then it should be ok to break the loop
        // cause it's not headers anymore
        continue;
      }

      // split this header line by ':', oh wait, we can use boost::split here
      size_t find_colon = other_line.find(':');
      // if no :, then ignore this line, but what if body also has :
      if (find_colon != string::npos) {
        // extract name and value
        string header_name = other_line.substr(0, find_colon);
        string header_value = other_line.substr(find_colon + 1);

        // trim all the white space
        boost::trim(header_name);
        boost::trim(header_value);

        // we don't really need it cause the AddhHeader will call tolower_copy
        boost::to_lower(header_name);

        // add this header to the header Map
        req.AddHeader(header_name, header_value);
      }

    }
  }


  return req;
}

}  // namespace hw4
