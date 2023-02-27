from flask import Flask, jsonify, make_response

app = Flask(__name__)


@app.route("/bad_content_length")
def bad_content_length():
    # here to change headers
    headers = {
        "Date": "Fri, 18 Sep 2020 12:53:30 GMT",
        "Cache-Control": "public, max-age=60",
        "Last-Modified": "Fri, 18 Sep 2020 12:53:30 GMT",
        "ETag": '"05dbbd20d38dec7598e4ae3e255200ad"',
        "Content-Type": "application/octet-stream",
        "Server": "AmazonS3",
        "Content-Length": "abcd",
    }
    response = make_response(jsonify(headers), 200)
    response.headers = headers

    return response


@app.route("/in_complete_response")
def in_complete_response():
    # here to change headers
    headers = {
        "Date": "Fri, 18 Sep 2020 12:53:30 GMT",
        "Cache-Control": "public, max-age=60",
        "Last-Modified": "Fri, 18 Sep 2020 12:53:30 GMT",
        "ETag": '"05dbbd20d38dec7598e4ae3e255200ad"',
        "Content-Type": "application/octet-stream",
        "Server": "AmazonS3",
        "Content-Length": "1",
    }
    response = make_response(jsonify(headers), 200)
    response.headers = headers

    return response


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
