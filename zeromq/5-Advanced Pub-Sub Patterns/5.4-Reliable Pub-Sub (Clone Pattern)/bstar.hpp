#pragma once

#include <zmqpp/zmqpp.hpp>

class bstar;
using callback_function = std::function<bool(zmqpp::socket_t&, bstar*)>;

class bstar
{
	// States we can be in at any point in time
	enum class state : int {
		STATE_PRIMARY,	// Primary, waiting for peer to connect
		STATE_BACKUP,	// Backup, waiting for peer to connect
		STATE_ACTIVE,	// Active - accepting connections
		STATE_PASSIVe	// Passive - not accepting connections
	};

	// Events, which start with the states our peer can be in
	enum class event : int {
		PEER_PRIMARY,	// HA peer is pending primary
		PEER_BACKUP,	// HA peer is pending backup
		PEER_ACTIVE,	// HA peer is active
		PEER_PASSIVE,	// HA peer is passive
		CLIENT_REQUEST	// client makes request
	};

	static constexpr int BSTAR_HEARTBEAT = 1000;

public:
	bstar(bool primary, std::string const& local, std::string const& remote):
		ctx_{},
		loop_{},
		statepub_(ctx_, zmqpp::socket_type::pub),
		statesub_(ctx_, zmqpp::socket_type::subscribe)
	{
		state_ = primary ? state::STATE_PRIMARY : state::STATE_BACKUP;

		// publisher
		statepub_.bind(local);

		// subscriber
		statesub_.set(zmqpp::socket_option::subscribe, "");
		statesub_.connect(remote);

		// set-up basic reactor events
		loop_.add(std::chrono::milliseconds(BSTAR_HEARTBEAT), 0, [this]{ 
			return bstar::send_state(this); 
			}
			);
		loop_.add(statesub_, [this]() {
			return bstar::recv_state(statesub_, this);
			});
	}

	int voter(std::string const& endpoint, zmqpp::socket_type type, callback_function handler, void* arg)
	{
		// hold actual handler+arg so we can call this later
		sockets_.push_back(std::make_unique<zmqpp::socket_t>(ctx_, type));
		auto& sock = sockets_.back();
		sock->bind(endpoint);
		voter_fn_ = handler;
		voter_arg_ = arg;
		loop_.add(*sock, [&sock, this]() {
			return voter_ready(*sock, this);
			});
		return 1;
	}

	// register handlers to be called each time there's a state change
	void new_active(callback_function handler, void* arg)
	{
		active_fn_ = handler;
		active_arg_ = arg;
	}

	void new_passive(callback_function handler, void* arg)
	{
		passive_fn_ = handler;
		passive_arg_ = arg;
	}

	int start()
	{
		update_peer_exipiry();
		loop_.start();
		return 1;
	}

private:
	bool execute()
	{
		bool rc = true;
		if (state::STATE_PRIMARY == state_) {
			if (event::PEER_BACKUP == event_) {
				fmt::print("I: connected to backup (passive), ready active\n");
				state_ = state::STATE_ACTIVE;
				if (active_fn_) {
					
				}
			}
		}

		return true;
	}

	void update_peer_exipiry()
	{
		peer_expiry_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(2 * BSTAR_HEARTBEAT);
	}

	// Reactor event handlers...

	// publish our state to peer
	static bool send_state(bstar* self)
	{
		self->statepub_.send(std::format("{}", static_cast<int>(self->state_)));
		return true;
	}

	// receive state from peer, execute finite state machine
	static bool recv_state(zmqpp::socket_t& socket, bstar* self)
	{
		std::string statestr;
		socket.receive(statestr);
		if (!statestr.empty()) {
			self->event_ = static_cast<bstar::event>(std::stoi(statestr));
			self->update_peer_exipiry();
		}
		self->execute();
		return true;
	}

	static bool voter_ready(zmqpp::socket_t& socket, bstar* self)
	{
		self->event_ = bstar::event::CLIENT_REQUEST;
		if (self->execute()) {
			self->voter_fn_(socket, self);
		}
		else {
			// destroy waiting message, no-one to read it
			zmqpp::message_t msg;
			socket.receive(msg);
		}
		return true;
	}

private:
	zmqpp::context_t	ctx_;
	zmqpp::loop			loop_;
	zmqpp::socket_t		statepub_;	// state publisher
	zmqpp::socket_t		statesub_;	// state subscriber
	bstar::state	state_;	// current state
	bstar::event	event_;	// current event
	std::chrono::steady_clock::time_point	peer_expiry_;	// when peer is considered 'dead'
	callback_function	voter_fn_;
	void* voter_arg_; // Arguments for voting handler
	callback_function	active_fn_; // call when become active
	void* active_arg_; // Arguments for handler
	callback_function	passive_fn_; // call when become passive
	void* passive_arg_; // Arguments for handler
	std::vector<std::unique_ptr<zmqpp::socket_t>> sockets_;
};