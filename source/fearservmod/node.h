#ifndef _NODE_H
#define _NODE_H

template <typename T> class nodeData {
public:
  ~nodeData() { clear(); }
  T *at(uintptr_t index) {
    T *pd = begin;
    int current = 0;
    if (pd) {
      while (pd) {
        T *temp = pd->next;
        if (current == index)
          return pd;
        pd = temp;
        ++current;
      }
    }
    return nullptr;
  }
  T* add() {
    T *pd = addNode();
    if (!pd)
      return nullptr;
    pd->next = begin;
    begin = pd;
    return pd;
  }
  void free(T *adr) {
    T *pd = begin;
    T *prevpd = 0;
    while (pd) {
      if (pd == adr) {
        if (prevpd)
          prevpd->next = pd->next;
        else
          begin = pd->next;
        freeNode(pd);
        return;
      }
      prevpd = pd;
      pd = pd->next;
    }
  }
  void clear() {
    T *pd = begin;
    if (pd) {
      while (pd) {
        T *temp = pd->next;
        freeNode(pd);
        pd = temp;
      }
    }
  }
  size_t count() {
    T *pd = begin;
    size_t cnt = 0;
    if (pd) {
      while (pd) {
        T *temp = pd->next;
        ++cnt;
        pd = temp;
      }
    }
    return cnt;
  }

private:
  inline void freeNode(T *pd) { free(pd); }
  inline T *addNode() {
    T *node = static_cast<T *>(malloc(sizeof(T)));
    memset(node,0, sizeof(T));
    return node;
  }
  T *begin = 0;
};
#endif
