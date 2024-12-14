#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <initializer_list>

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

    // Шаблон класса «Базовый Итератор».
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList<Type>;

        explicit BasicIterator(Node* node) {
            node_ = node;
        }

        public:

        // Категория итератора — forward iterator
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<value_type>& other) noexcept
        : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_==rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_==rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
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
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:

    SingleLinkedList()
    : head_(),
    size_(0u)
    {
    }

    explicit SingleLinkedList(std::initializer_list<Type> values);

    explicit SingleLinkedList(const SingleLinkedList& other);

    ~SingleLinkedList() noexcept {
        Clear();
    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept;

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept;

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value);

    // Очищает список за время O(N)
    void Clear() noexcept;

    void PopFront() noexcept;

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }


    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            // Реализация операции присваивания с помощью идиомы Copy-and-swap.
            // Если исключение будет выброшено, то на текущий объект оно не повлияет.
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }
  
    void swap(SingleLinkedList& other) noexcept;

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node *>(&head_)};
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node *new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_; 
        return Iterator{new_node};
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node *temp = pos.node_->next_node;
        pos.node_->next_node = temp->next_node;
        delete temp;
        --size_;
        return Iterator{pos.node_->next_node};
    }


private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    std::size_t size_;
};

template <typename Type>
SingleLinkedList<Type>::SingleLinkedList(std::initializer_list<Type> values) {
    auto iter = values.begin() + (values.size() - 1);
    size_t values_size = values.size();
    for(size_t i = 0; i < values_size; ++i)
    {
        PushFront(*iter);
        --iter;
    }
    size_ = values.size();
}

template <typename Type>
SingleLinkedList<Type>::SingleLinkedList(const SingleLinkedList<Type>& other) 
    : head_(), size_(0) {
    if (other.IsEmpty()) {
        return;
    }
    try
    {
        SingleLinkedList<Type>::Node* current = &head_; // Текущий узел для записи
        for (Node* other_node = other.head_.next_node; other_node != nullptr; other_node = other_node->next_node) {
            current->next_node = new SingleLinkedList<Type>::Node(other_node->value, nullptr);  // Создаём новый узел
            current = current->next_node;  // Переходим к следующему узлу
            ++size_;  // Увеличиваем счётчик элементов
        }
    }
    catch(...)
    {
        Clear();
        throw;
    }
}

template<typename Type>
[[nodiscard]] size_t SingleLinkedList<Type>::GetSize() const noexcept {
    return size_;
}

template<typename Type>
[[nodiscard]] bool SingleLinkedList<Type>::IsEmpty() const noexcept {
    return (size_ == 0);
}

template<typename Type>
void SingleLinkedList<Type>::PushFront(const Type& value) {
    SingleLinkedList<Type>::Node *first_node = new SingleLinkedList<Type>::Node(value, head_.next_node);
    head_.next_node = first_node;
    ++size_;
}

template<typename Type>
void SingleLinkedList<Type>::Clear() noexcept {
while(size_ > 0)
    {
        SingleLinkedList<Type>::Node *first_node = head_.next_node;
        head_.next_node = first_node->next_node;
        delete first_node;
        --size_;
    }
}

template<typename Type>
void SingleLinkedList<Type>::PopFront() noexcept {
    SingleLinkedList<Type>::Node *temp = head_.next_node;
    head_.next_node = temp->next_node;
    delete temp;
    --size_;
}

template<typename Type>
void SingleLinkedList<Type>::swap(SingleLinkedList& other) noexcept {
    std::swap(this->head_.next_node, other.head_.next_node);
    std::swap(this->size_, other.size_);
}

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), [](const Type& l_value, const Type& r_value){
        return l_value == r_value;
    });
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), [](const Type& lhs_el, const Type& rhs_el){
        return lhs_el > rhs_el;
    });
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), [](const Type& lhs_el, const Type& rhs_el){
        return lhs_el < rhs_el;
    }) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs<=rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs<rhs);
}

