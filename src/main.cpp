#include <iostream>

#include <systemc>
#include <vector>


using namespace sc_core;

struct testit: public sc_module
{
    SC_HAS_PROCESS(testit);

    struct pkt_t {
        sc_time delivery;
        int value;
    };


    testit(sc_core::sc_module_name mn){
        SC_THREAD(receiver);
        SC_THREAD(generator);

    }



    void receiver()
    {
        while (true)
        {
            wait(delivery_time);

            auto& pkt = rx_pkts[0];

            std::cout << "RXX at " << sc_time_stamp() << " Pkt del-time:" << pkt.delivery << " value "<<pkt.value <<"\n";

            rx_pkts.erase(rx_pkts.begin());


            if (!rx_pkts.empty())
            {
                delivery_time.notify(rx_pkts.front().delivery - sc_time_stamp());
            }
        }
    }

    void generator()
    {
        struct inject_t {
            sc_time delay;
            sc_time receive_time;
            int value;
        };

        std::vector<inject_t> samples {
            {{0,   SC_NS}, {100, SC_NS}, 10},
            {{100, SC_NS}, {200, SC_NS}, 20},
            {{50 , SC_NS}, {250, SC_NS}, 30},
            {{50 , SC_NS}, {300, SC_NS}, 40},
            {{70 , SC_NS}, {280, SC_NS}, 50},
            {{60 , SC_NS}, {340, SC_NS}, 60},
        };


        for(auto& sam: samples)
        {
            wait(sam.delay);
            std::cout << "INJECT: at " << sc_time_stamp() << " data:" << sam.value << " for " << sam.receive_time << "\n";
            do_tx(sam.receive_time, sam.value);
        }

    }

    void do_tx(const sc_time receive_time, int value)
    {
        //std::cout << "TX Operation: " << receive_time << " value:" << value << "\n";

        if (rx_pkts.empty()) {
            rx_pkts.push_back({receive_time, value});
            delivery_time.notify(receive_time - sc_time_stamp());
        } else {
            bool update = rx_pkts.front().delivery > receive_time;

            bool inserted = false;
            for (auto itr = rx_pkts.begin(); itr!=rx_pkts.end(); itr++) {
                if ((*itr).delivery > receive_time)
                {
                    rx_pkts.insert(itr, {receive_time, value});
                    inserted = true;
                    break;
                }
            }

            if (!inserted)
            {
                rx_pkts.push_back({receive_time, value});
            }

            if (update)
            {
                delivery_time.notify(receive_time - sc_time_stamp());
            }
        }
    }

    sc_event delivery_time;

    std::vector<pkt_t> rx_pkts;
};

int sc_main(int argc, char **argv)
{
	testit dut("Zala");

    sc_core::sc_start(1, SC_SEC);


	std::cout << "Done....\n";
	return 0;
}
