
class handleData {
  static handleData *object;
  handleData();
  handleData(const handleData &) = delete;
  handleData &operator=(const handleData &) = delete;
  void init(void *hModHandle);

 public:
  static handleData *instance();
  ~handleData();
  static unsigned stdcall handleInit(void *hModHandle);
  static unsigned stdcall handleClientThread();
  void detach();

  appData aData;
  fearData pSdk;
};
