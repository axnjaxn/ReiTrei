/*
 * circlelist.h
 * A circular linked list implementation by Brian Jackson
 * 17 October 2008
 * Modified 10 March 2010
 * Deprecated 6 March 2014
 */

#ifndef bpj_circlelist_h
#define bpj_circlelist_h

template<typename tn>
class CircleListItem {
 public:
  tn data;
  CircleListItem<tn>* next;

  CircleListItem(const tn& t, CircleListItem* n = NULL) {data = t; next = n;}
};

template<typename tn>
class CircleList {
 protected:
  CircleListItem<tn>* tail;
  int count;

 public:
  CircleList() {tail = NULL; count = 0;}
  virtual ~CircleList() {while (count > 0) pop_front();}

  CircleList(const CircleList& cl) {
    tail = NULL;
    count = 0;

    append(cl);
  }

  void deleteAll() {
    while (count > 0) {
      delete front();
      pop_front();
    }
  }
  inline bool empty() const {return count == 0;}
  inline int size() const {return count;}

  inline tn& front() {return tail->next->data;}
  inline tn front() const {return tail->next->data;}
  inline tn& back() {return tail->data;}
  inline tn back() const {return tail->data;}

  void push_front(const tn& t) {
    CircleListItem<tn>* item = new CircleListItem<tn>(t);
    if (count == 0) {
      tail = item;
      item->next = item;
    }
    else {
      item->next = tail->next;
      tail->next = item;
    }
    count++;
  }
  void push_back(const tn& t) {
    push_front(t);
    tail = tail->next;
  }
  void pop_front() {
    CircleListItem<tn>* dead = tail->next;
    tail->next = dead->next;
    delete dead;
    count--;
  }
  inline void rotate() {tail = tail->next;}
  tn& operator[](int ind) {
    CircleListItem<tn>* scan = tail->next;
    for (int i = 0; i < ind; i++)
      scan = scan->next;
    return scan->data;
  }
  void remove(int ind) {
    CircleListItem<tn>* dead;
    if (count == 1) {
      dead = tail;
      tail = NULL;
    }
    else if (ind == 0) {
      dead = tail->next;
      tail->next = dead->next;
    }
    else {
      CircleListItem<tn>* scan = tail;
      for (int i = 0; i < ind; i++)
	scan = scan->next;
      dead = scan->next;
      if (dead == tail) tail = scan;
      scan->next = dead->next;
    }
    delete dead;
    count--;
  }
  void append(const CircleList<tn>& list) {
    if (list.empty()) return;

    CircleListItem<tn>* scan = list.tail->next;

    for(int i = 0; i < list.size(); i++) {
      push_back(scan->data);
      scan = scan->next;
    }
  }
};

#endif
