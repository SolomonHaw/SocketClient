#include <array>
#include <iostream>
#include <boost/asio.hpp>
 
using boost::asio::ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main() {
	boost::asio::io_context ctn;
	tcp::endpoint ep(boost::asio::ip::address_v4::from_string("127.0.0.1"), 1234);

	tcp::socket sock(ctn);
	sock.connect(ep);

	//return message on intercept msg
	const string msg = "Hello Server!\n";
	boost::system::error_code erc;
	boost::asio::write(sock, boost::asio::buffer(msg), erc);
	if (!erc) {
		cout << "Client said hello!" << endl;
	}
	else {
		cout << "write failed: " << erc.message() << endl;
	}

	//Get response from the server
	boost::asio::streambuf recv_buf;
	boost::asio::read_until(sock, recv_buf, "\n");
	if (erc && erc != boost::asio::error::eof) {
		cout << "receive failed: " << erc.message() << endl;
	}
	else {
		//common safe asio technique to obtain the pointer to a buffer
		const char* data = boost::asio::buffer_cast<const char*>(recv_buf.data());
		cout << "Recieved: " << data << endl;
	}
	return 0;
}