#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <stdexcept>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;
        explicit BasicIterator(Node* node) : node_(node) {
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(this != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(this != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:

    SingleLinkedList() : head_(), size_(0u) {
    };

    ~SingleLinkedList() {
        Clear();
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        for (auto it = values.end() - 1; it >= values.begin(); --it) {
            PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList tmp;

        for (auto it = other.begin(); it != other.end(); it++) {
            tmp.PushBack(*it);
        }
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        assert(head_.next_node != rhs.head_.next_node);
        SingleLinkedList tmp(rhs);
        swap(tmp);
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);
    }

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return BasicIterator<Type>{ head_.next_node };
    }

    [[nodiscard]] Iterator end() noexcept {
        return BasicIterator<Type>();
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return BasicIterator<const Type>{ head_.next_node };
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return BasicIterator<Type>();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return BasicIterator<const Type>{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return BasicIterator<Type>();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return BasicIterator<Type>{ &head_ };
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return BasicIterator<Type>{ const_cast<Node*>(&head_) };
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return BasicIterator<const Type>{ &head_ };
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0u ? true : false;
    }

    // Вставка в конец
    void PushBack(const Type& value) {
        if (head_.next_node == nullptr) {
            head_.next_node = new Node(value, nullptr);
        }
        else {
            Node* current = head_.next_node;
            while (current->next_node != nullptr)
            {
                current = current->next_node;
            }
            current->next_node = new Node(value, nullptr);
        }
        ++size_;
    }

    // Вставка в начало
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // удаление элемента с конца
    void PopBack() {
        Node* current = head_.next_node;
        while (current->next_node->next_node != nullptr)
        {
            current = current->next_node;
        }

        Node* buffer = current->next_node;
        current->next_node = nullptr;
        delete buffer;

        --size_;
    }

    // удаление элемента с начала
    void PopFront() noexcept {
        Node* buffer = head_.next_node;
        head_.next_node = head_.next_node->next_node;
        delete buffer;

        --size_;
    }

    // вставка элемента в список на место по индексу, со смещением остальных значений
    void Insert(const int pos, const Type& value) {
        if (pos == 0) {
            PushFront(value);
        }
        else if (pos == size_) {
            PushBack(value);
        }
        else {
            try {
                if (pos > size_) {
                    throw pos;
                }

                int counter = 0;
                Node* buffer = &head_;
                while (buffer->next_node != nullptr)
                {
                    if (counter == pos) {
                        break;
                    }
                    else {
                        buffer = buffer->next_node;
                        ++counter;
                    }
                }
                buffer->next_node = new Node(value, buffer->next_node);
                ++size_;
            }
            catch (const int& ex) {
                std::cout << "Position [" << ex << "] is out of range" << std::endl;
                std::cout << "Passing data to PushBack()" << std::endl;
                PushBack(value);
            }
        }
    }

    // удаление элемента списка с позицией по индексу
    void Remove(const int pos) {
        if (pos == 0) {
            PopFront();
        }
        else if (pos == size_ - 1) {
            PopBack();
        }
        else {
            try {
                if (pos > GetSize()) {
                    throw pos;
                }

                int counter = 0;
                Node* buffer = &head_;
                while (buffer->next_node != nullptr)
                {
                    if (counter == pos) {
                        break;
                    }
                    else {
                        buffer = buffer->next_node;
                        ++counter;
                    }
                }
                Node* removed_element = buffer->next_node;
                buffer->next_node = buffer->next_node->next_node;
                delete removed_element;
                --size_;
            }
            catch (const int& ex) {
                std::cout << "Position [" << ex << "] is out of range" << std::endl;
                std::cout << "Passing request to PopBack()" << std::endl;
                PopBack();
            }
        }
    }

    // вставить перед, возвращает итератор на вставленный элемент
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* insert_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = insert_node;
        ++size_;
        return BasicIterator<Type>{ insert_node };
    }

    // удалить после, возвращает итератор на следующее значение после удаленного элемента
    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos == before_begin() || pos == cbefore_begin()) {
            PopFront();
        }
        else {
            Node* buffer = pos.node_->next_node;
            pos.node_->next_node = buffer->next_node;
            delete buffer;

            --size_;
        }
        return BasicIterator<Type>{ const_cast<Node*>(pos.node_->next_node) };
    }

    // Печать значений листа в линию для стандартных типов
    void PrintInLine() {
        try {
            if (this->begin() == this->end()) {
                throw size_;
            }

            std::cout << "List Values : ";
            for (auto it = this->begin(); it != this->end(); it++) {
                std::cout << *it << ' ';
            }
            std::cout << '\n';
        }
        catch (const size_t&) {
            std::cout << "List can't be printed \nList is Empty" << std::endl;
        }
    }

    // Очистка списка
    void Clear() {
        while (head_.next_node != nullptr)
        {
            auto ptr_buffer = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete ptr_buffer;
        }

        if (head_.next_node == nullptr) {
            size_ = 0u;
        }
    }

    // доступ по квадратным скобкам
    Type& operator[](const int index) {
        try {
            if (index >= GetSize() || index < 0) {
                throw index;
            }

            int counter = 0;
            Node* buffer = head_.next_node;
            while (buffer->next_node != nullptr)
            {
                if (counter == index) {
                    return buffer->value;
                }
                else {
                    buffer = buffer->next_node;
                    ++counter;
                }
            }
        }
        catch (const int& ex) {
            std::cout << "Index [" << ex << "] is out of range" << std::endl;
            std::terminate();
        }
    }

private:
    Node head_;
    size_t size_ = 0u;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs == rhs) {
        return lhs == rhs;
    }
    else {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs == rhs) {
        return lhs == rhs;
    }
    else {
        return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
}
