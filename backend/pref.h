#ifndef PREF_H
#define PREF_H

class pref {
public:
	static void load_preference(int argc, char* argv[]);
	static void run_io_engine();
	static pref& instance() { return instance_; }

private:
	static pref instance_;
};

#endif
