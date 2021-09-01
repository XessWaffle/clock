class Digit{
  public:
    Digit();
    Digit(char num);

    char getNumber();
    void setNumber(char num);

    bool isHexRepresentation();
    void setHex(bool hex);

    void custom(char converted);

    char convert();
    bool converted();
    
  private:
    char _num;
    char _convert;
    bool _converted = false;
    bool _hex = false;  
};
