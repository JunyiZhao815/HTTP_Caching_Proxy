from flask import Flask, jsonify, make_response

app = Flask(__name__)


@app.route("/")
def example():
    # here to change headers
    headers = {
        "Content-Type": "application/json",
        "Cache-Control": "max-age=360000",
    }
    response = make_response(jsonify(headers), 200)
    response.headers = headers

    return response


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
