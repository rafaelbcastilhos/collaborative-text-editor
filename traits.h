#define num_lines 500
#define line_size 100
#define max_clients 10

#define GET 0
#define ADD 1

struct message {
  char text[num_lines];
  int idx;
  int type;
};