/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/tcp_receiver.hpp"
#include "ciri/node.h"
#include "common/network/neighbor.h"
#include "utils/logger_helper.h"

#define TCP_RECEIVER_SERVICE_LOGGER_ID "tcp_receiver_service"

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(receiver_service_t* const service,
                             boost::asio::ip::tcp::socket socket)
    : service_(service), socket_(std::move(socket)) {}

TcpConnection::~TcpConnection() {
  if (neighbor_ != NULL) {
    log_info(TCP_RECEIVER_SERVICE_LOGGER_ID,
             "Connection lost with tethered neighbor tcp://%s:%d\n",
             socket_.remote_endpoint().address().to_string().c_str(),
             socket_.remote_endpoint().port());
    neighbor_->endpoint.opaque_inetaddr = NULL;
  }
}

void TcpConnection::start() {
  auto host = socket_.remote_endpoint().address().to_string().c_str();
  auto port = socket_.remote_endpoint().port();

  neighbor_ = neighbor_find_by_values(service_->state->node->neighbors,
                                      PROTOCOL_TCP, host, port);
  if (neighbor_ == NULL) {
    log_info(TCP_RECEIVER_SERVICE_LOGGER_ID,
             "Connection denied with non-tethered neighbor tcp://%s:%d\n", host,
             port);
    return;
  }
  log_info(TCP_RECEIVER_SERVICE_LOGGER_ID,
           "Connection accepted with tethered neighbor tcp://%s:%d\n", host,
           port);
  neighbor_->endpoint.opaque_inetaddr = &socket_;
  receive();
}

void TcpConnection::receive() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket_, boost::asio::buffer(packet_.content, TRANSACTION_PACKET_SIZE),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          handlePacket(length);
          receive();
        }
      });
}

bool TcpConnection::handlePacket(std::size_t const length) {
  if (length != TRANSACTION_PACKET_SIZE) {
    return false;
  }
  receiver_service_prepare_packet(
      &packet_, length, socket_.remote_endpoint().address().to_string().c_str(),
      socket_.remote_endpoint().port(), PROTOCOL_TCP);
  log_debug(TCP_RECEIVER_SERVICE_LOGGER_ID,
            "Packet received from tethered neighbor tcp://%s:%d\n",
            &packet_.source.host, packet_.source.port);
  if (service_->queue->vtable->push(service_->queue, packet_) !=
      CONCURRENT_QUEUE_SUCCESS) {
    return false;
  }
  return true;
}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(receiver_service_t* const service,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service),
      acceptor_(context, boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(), port)) {
  logger_helper_init(TCP_RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  accept();
}

TcpReceiverService::~TcpReceiverService() {
  logger_helper_destroy(TCP_RECEIVER_SERVICE_LOGGER_ID);
}

void TcpReceiverService::accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<TcpConnection>(service_, std::move(socket))->start();
    }
    accept();
  });
}
