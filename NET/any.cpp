class   {
    private:
        class Holder {
            public:
                virtual ~holder() {}
                virtual std::type_info type() = 0;
                virtual holder * clone() = 0;
        }
}