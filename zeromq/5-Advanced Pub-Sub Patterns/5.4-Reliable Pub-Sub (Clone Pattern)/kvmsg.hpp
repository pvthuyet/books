#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include <sstream>
#include <charconv>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fmt/color.h>
#include <zmqpp/zmqpp.hpp>

class kvmsg
{
    using Properties = std::unordered_map<std::string, std::string>;

private:
	//  Keys are short strings
	static constexpr int  KVMSG_KEY_MAX = 255;

    //  Message is formatted on wire as 4 frames:
    //  frame 0: getKey (0MQ string)
    //  frame 1: getSequence (8 bytes, network order)
    //  frame 2: uuid (blob, 16 bytes)
    //  frame 3: properties (0MQ string)
    //  frame 4: body (blob)
    static constexpr int  FRAME_KEY = 0;
    static constexpr int  FRAME_SEQ = 1;
    static constexpr int  FRAME_UUID = 2;
    static constexpr int  FRAME_PROPS = 3;
    static constexpr int  FRAME_BODY = 4;
    static constexpr int  KVMSG_FRAMES = 5;

    //  Presence indicators for each frame
    std::array<bool, KVMSG_FRAMES> present;
    //  Corresponding 0MQ message frames, if any
    std::array<std::string, KVMSG_FRAMES> frame;
    // key, copied into safe string
    std::string key;
    // list of properties, as name=value strings
    Properties props;

private:
    //  .split property encoding
    //  These two helpers serialize a list of properties to and from a
    //  message frame:
    void encodeProps()
    {
        std::stringstream ss;
        std::string msg;
        for (auto& [k,v] : props) {
            if (!msg.empty()) {
                msg.append(1, '\n');
            }
            msg += std::format("{}={}", k, v);
        }
        boost::archive::text_oarchive oa(ss);
        oa << msg;
        present[FRAME_PROPS] = true;
        frame[FRAME_PROPS] = ss.str();
    }

    void decodeProps()
    {
        auto msg = frame[FRAME_PROPS];
        props.clear();
        if (0 == msg.length()) return;

        fmt::print("[decodeProps] original: {}\n", msg);
        std::stringstream ss(msg);
        boost::archive::text_iarchive ia(ss);
        std::string demsg;
        ia >> demsg;
        std::vector<std::string> res;
        boost::split(res, demsg, boost::is_any_of("\n"));
        for (auto const& e : res) {
            std::vector<std::string> res2;
            boost::split(res2, e, boost::is_any_of("="));
            if (2 == res2.size()) {
                props.insert(std::make_pair(res2[0], res2[1]));
            }
        }
    }

public:
    kvmsg(int seq)
    {
        setSequence(seq);
    }

    //  .split recv method
        //  This method reads a getKey-value message from the socket and returns a
        //  new {{kvmsg}} instance:
    bool recv(zmqpp::socket_t& socket)
    {
        //  This method is almost unchanged from kvsimple
        //  .skip
        zmqpp::message_t msg;
        socket.receive(msg);
        if (msg.parts() < KVMSG_FRAMES) return false;

        //  Read all frames off the wire, reject if bogus
        int frameNbr;
        for (frameNbr = 0; frameNbr < KVMSG_FRAMES; frameNbr++) {
            this->present[frameNbr] = true;
            msg >> frame[frameNbr];
        }

        //  .until
        this->decodeProps();
        return true;
    }

    //  Send getKey-value message to socket; any empty frames are sent as such.
    void send(zmqpp::socket_t& socket)
    {
        zmqpp::message_t msg;
        encodeProps();
        //  The rest of the method is unchanged from kvsimple
        //  .skip
        int frameNbr;
        for (frameNbr = 0; frameNbr < KVMSG_FRAMES; frameNbr++) {
            std::string content;
            if (present[frameNbr])
                content = frame[frameNbr];
            msg << content;
        }
        socket.send(msg);
    }

    //  .until

    //  .split dup method
    //  This method duplicates a {{kvmsg}} instance, returns the new instance:
    kvmsg dup()
    {
        return kvmsg(*this);
    }
    //  The getKey, getSequence, body, and size methods are the same as in kvsimple.
    //  .skip


    //  Return getKey from last read message, if any, else NULL
    std::string getKey()
    {
        if (present[FRAME_KEY]) {
            return frame[FRAME_KEY];
        }
        return {};
    }

    //  Set message getKey as provided
    void setKey(std::string const& key)
    {
        frame[FRAME_KEY] = key;
        present[FRAME_KEY] = true;
    }

    //  Return getSequence nbr from last read message, if any
    int getSequence()
    {
        if (present[FRAME_SEQ]) {
            assert(frame[FRAME_SEQ].length() == 8);
            int res{};
            auto [p, ec] = std::from_chars(
                std::data(frame[FRAME_SEQ]), 
                std::data(frame[FRAME_SEQ]) + std::size(frame[FRAME_SEQ]), 
                res);
            return res;
        }

        return 0;
    }

    //  Set message getSequence number
    void setSequence(int sequence)
    {
        present[FRAME_SEQ] = true;
        frame[FRAME_SEQ] = std::format("{:08d}", sequence);
    }

    //  Return body from last read message, if any, else NULL
    std::string body()
    {
        if (present[FRAME_BODY])
            return frame[FRAME_BODY];
        return {};
    }

    //  Set message body
    void setBody(std::string const& body)
    {
        frame[FRAME_BODY] = body;
        present[FRAME_BODY] = true;
    }

    //  Return body size from last read message, if any, else zero
    int size()
    {
        if (present[FRAME_BODY])
            return frame[FRAME_BODY].length();
        return 0;
    }
    //  .until

    //  .split UUID methods
    //  These methods get and set the UUID for the getKey-value message:
    std::string UUID()
    {
        if (present[FRAME_UUID])
            return frame[FRAME_UUID];
        return {};
    }

    //  Sets the UUID to a randomly generated value
    void setUUID()
    {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        frame[FRAME_UUID] = boost::lexical_cast<std::string>(uuid);
    }

    //  .split property methods
    //  These methods get and set a specified message property:

    //  Get message property, return "" if no such property is defined.
    std::string getProp(std::string const& name)
    {
        if (props.count(name)) return props.at(name);
        return {};
    }

    //  Set message property. Property name cannot contain '='. Max length of
    //  value is 255 chars.
    void setProp(std::string const& name, std::string const& value)
    {
        props.insert_or_assign(name, value);
    }

    //  .split store method
    //  This method stores the getKey-value message into a hash map, unless
    //  the getKey and value are both null. It nullifies the {{kvmsg}} reference
    //  so that the object is owned by the hash map, not the caller:

    void store(std::unordered_map<std::string, kvmsg>& hash)
    {
        if (size() > 0) {
            if (present[FRAME_KEY] && present[FRAME_BODY]) {
                hash.insert_or_assign(getKey(), *this);
            }
        }
        else {
            hash.erase(getKey());
        }
    }

    //  .split dump method
    //  This method extends the {{kvsimple}} implementation with support for
    //  message properties:

    void dump()
    {
        auto sz = size();
        auto bd = body();
        fmt::print("[seq:{}]\n", getSequence());
        fmt::print("[getKey:{}]\n", getKey());
        //  .until
        fmt::print("[size:{}] \n", sz);
        fmt::print("[");
        for (auto& [k,v] : props) {
            fmt::print("{}={};", k, v);
        }
        fmt::print("]\n");

        //  .skip
        for (int charNbr = 0; charNbr < sz; charNbr++)
            fmt::print("{0:#x} ", bd[charNbr]);
        fmt::print("\n");
    }

    //  .until

    //  .split test method
    //  This method is the same as in {{kvsimple}} with added support
    //  for the uuid and property features of {{kvmsg}}:
    static void test(bool verbose)
    {
        fmt::print(" * kvmsg: \n");

        //  Prepare our context and sockets
        try {
            zmqpp::context_t ctx{};
            zmqpp::socket_t output(ctx, zmqpp::socket_type::dealer);
            output.bind("tcp://*:5555");

            zmqpp::socket_t input(ctx, zmqpp::socket_type::dealer);
            input.connect("tcp://localhost:5555");

            std::unordered_map<std::string, kvmsg> kvmap{};

            //  .until
            //  Test send and receive of simple message
            kvmsg msg(1);
            msg.setKey("getKey");
            msg.setUUID();
            msg.setBody("body");
            if (verbose)
                msg.dump();
            msg.send(output);
            msg.store(kvmap);

            kvmsg reply(0);
            reply.recv(input);
            if (verbose)
                reply.dump();
            assert(reply.getKey() == "getKey");
            reply.store(kvmap);

            //  Test send and receive of message with properties
            kvmsg msg2(2);
            msg2.setProp("prop1", "value1");
            msg2.setProp("prop2", "value1");
            msg2.setProp("prop2", "value2");
            msg2.setKey("getKey");
            msg2.setUUID();
            msg2.setBody("body");
            assert(msg2.getProp("prop2") == "value2");
            if (verbose)
                msg2.dump();
            msg2.send(output);

            kvmsg reply2{ 0 };
            reply2.recv(input);
            if (verbose)
                reply2.dump();
            assert(reply2.getKey() == "getKey");
            assert(reply2.getProp("prop2") == "value2");
        }
        catch (std::exception const& ex) {
            fmt::print("[error] {}", ex.what());
        }
        fmt::print("OK\n");
    }
    //  .until
};