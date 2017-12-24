void *operator new(unsigned sz);
void operator delete(void *addr);

namespace stl {

struct wstring {
  wchar_t *data;
  int length;
  int capacity;
};

struct string {
  wchar_t *data;
  int length;
  int capacity;
};
}  // namespace stl