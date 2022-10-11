#define max_clients 10
#define num_lines 500
#define line_size 100
#define GET 0
#define ADD 1
#define EXIT 2

struct message {
  char text[line_size];
  int idx;
  int type;
};
