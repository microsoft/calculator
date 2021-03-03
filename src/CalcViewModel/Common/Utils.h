// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/ExpressionCommandInterface.h"
#include "DelegateCommand.h"
#include "GraphingInterfaces/GraphingEnums.h"

// Utility macros to make Models easier to write
// generates a member variable called m_<n>

#define SINGLE_ARG(...) __VA_ARGS__

#define PROPERTY_R(t, n)                                                                                                                                       \
    property t n                                                                                                                                               \
    {                                                                                                                                                          \
        t get()                                                                                                                                                \
        {                                                                                                                                                      \
            return m_##n;                                                                                                                                      \
        }                                                                                                                                                      \
                                                                                                                                                               \
    private:                                                                                                                                                   \
        void set(t value)                                                                                                                                      \
        {                                                                                                                                                      \
            m_##n = value;                                                                                                                                     \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    t m_##n;                                                                                                                                                   \
                                                                                                                                                               \
public:

#define PROPERTY_RW(t, n)                                                                                                                                      \
    property t n                                                                                                                                               \
    {                                                                                                                                                          \
        t get()                                                                                                                                                \
        {                                                                                                                                                      \
            return m_##n;                                                                                                                                      \
        }                                                                                                                                                      \
        void set(t value)                                                                                                                                      \
        {                                                                                                                                                      \
            m_##n = value;                                                                                                                                     \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    t m_##n;                                                                                                                                                   \
                                                                                                                                                               \
public:

#define OBSERVABLE_PROPERTY_R(t, n)                                                                                                                            \
    property t n                                                                                                                                               \
    {                                                                                                                                                          \
        t get()                                                                                                                                                \
        {                                                                                                                                                      \
            return m_##n;                                                                                                                                      \
        }                                                                                                                                                      \
                                                                                                                                                               \
    private:                                                                                                                                                   \
        void set(t value)                                                                                                                                      \
        {                                                                                                                                                      \
            if (m_##n != value)                                                                                                                                \
            {                                                                                                                                                  \
                m_##n = value;                                                                                                                                 \
                RaisePropertyChanged(L#n);                                                                                                                     \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    t m_##n;                                                                                                                                                   \
                                                                                                                                                               \
public:

#define OBSERVABLE_PROPERTY_RW(t, n)                                                                                                                           \
    property t n                                                                                                                                               \
    {                                                                                                                                                          \
        t get()                                                                                                                                                \
        {                                                                                                                                                      \
            return m_##n;                                                                                                                                      \
        }                                                                                                                                                      \
        void set(t value)                                                                                                                                      \
        {                                                                                                                                                      \
            if (m_##n != value)                                                                                                                                \
            {                                                                                                                                                  \
                m_##n = value;                                                                                                                                 \
                RaisePropertyChanged(L#n);                                                                                                                     \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    t m_##n;                                                                                                                                                   \
                                                                                                                                                               \
public:

#define OBSERVABLE_NAMED_PROPERTY_R(t, n)                                                                                                                      \
    OBSERVABLE_PROPERTY_R(t, n)                                                                                                                                \
public:                                                                                                                                                        \
    static property Platform::String ^ n##PropertyName                                                                                                         \
    {                                                                                                                                                          \
        Platform::String ^ get() { return Platform::StringReference(L#n); }                                                                                    \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define OBSERVABLE_NAMED_PROPERTY_RW(t, n)                                                                                                                     \
    OBSERVABLE_PROPERTY_RW(t, n)                                                                                                                               \
public:                                                                                                                                                        \
    static property Platform::String ^ n##PropertyName                                                                                                         \
    {                                                                                                                                                          \
        Platform::String ^ get() { return Platform::StringReference(L#n); }                                                                                    \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define OBSERVABLE_PROPERTY_FIELD(n) m_##n

// This variant of the observable object is for objects that don't want to react to property changes
#ifndef UNIT_TESTS
#define OBSERVABLE_OBJECT()                                                                                                                                    \
    virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;                                                                      \
    internal:                                                                                                                                                  \
    void RaisePropertyChanged(Platform::String ^ p)                                                                                                            \
    {                                                                                                                                                          \
        PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(p));                                                                   \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:
#else
#define OBSERVABLE_OBJECT()                                                                                                                                    \
    virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;                                                                      \
    internal:                                                                                                                                                  \
    void RaisePropertyChanged(Platform::String ^ p)                                                                                                            \
    {                                                                                                                                                          \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:
#endif

// The callback specified in the macro is a method in the class that will be called every time the object changes
// the callback is supposed to be have a single parameter of type Platform::String^
#ifndef UNIT_TESTS
#define OBSERVABLE_OBJECT_CALLBACK(c)                                                                                                                          \
    virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;                                                                      \
    internal:                                                                                                                                                  \
    void RaisePropertyChanged(Platform::String ^ p)                                                                                                            \
    {                                                                                                                                                          \
        PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(p));                                                                   \
        c(p);                                                                                                                                                  \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:
#else
#define OBSERVABLE_OBJECT_CALLBACK(c)                                                                                                                          \
    virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;                                                                      \
    internal:                                                                                                                                                  \
    void RaisePropertyChanged(Platform::String ^ p)                                                                                                            \
    {                                                                                                                                                          \
        c(p);                                                                                                                                                  \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:
#endif

// The variable member generated by this macro should not be used in the class code, use the
// property getter instead.
#define COMMAND_FOR_METHOD(p, m)                                                                                                                               \
    property Windows::UI::Xaml::Input::ICommand^ p {\
    Windows::UI::Xaml::Input::ICommand^ get() {\
    if (!donotuse_##p) {\
    donotuse_##p = CalculatorApp::Common::MakeDelegate(this, &m);\
    } return donotuse_##p; }} private: Windows::UI::Xaml::Input::ICommand^ donotuse_##p;                                                                                                         \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_DECLARATION(t, n)                                                                                                                  \
    property t n                                                                                                                                               \
    {                                                                                                                                                          \
        t get()                                                                                                                                                \
        {                                                                                                                                                      \
            return safe_cast<t>(GetValue(s_##n##Property));                                                                                                    \
        }                                                                                                                                                      \
        void set(t value)                                                                                                                                      \
        {                                                                                                                                                      \
            SetValue(s_##n##Property, value);                                                                                                                  \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##n##Property;                                                                                            \
                                                                                                                                                               \
private:

// Utilities for DependencyProperties
namespace Utils
{
    namespace Details
    {
        template <typename T>
        struct IsRefClass
        {
            static const bool value = __is_ref_class(T);
        };

        template <typename T>
        struct RemoveHat
        {
            typedef T type;
        };

        template <typename T>
        struct RemoveHat<T ^>
        {
            typedef T type;
        };

        template <typename T>
        typename std::enable_if<IsRefClass<T>::value, T ^>::type MakeDefault()
        {
            return nullptr;
        }

        template <typename T>
        typename std::enable_if<!IsRefClass<T>::value, T>::type MakeDefault()
        {
            return T();
        }

        // There's a bug in Xaml in which custom enums are not recognized by the property system/binding
        // therefore this template will determine that for enums the type to use to register the
        // DependencyProperty is to be Object, for everything else it will use the type
        // NOTE: If we are to find more types in which this is broken this template
        // will be specialized for those types to return Object
        template <typename T>
        struct TypeToUseForDependencyProperty
        {
            typedef typename std::conditional<std::is_enum<T>::value, Platform::Object, T>::type type;
        };
    }

    // Regular DependencyProperty
    template <typename TOwner, typename TType>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyProperty(
        _In_ const wchar_t* const name,
        _In_ Windows::UI::Xaml::PropertyMetadata^ metadata)
    {
        typedef typename Details::RemoveHat<TOwner>::type OwnerType;
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        typedef typename Details::TypeToUseForDependencyProperty<ThisPropertyType>::type ThisDependencyPropertyType;

        static_assert(Details::IsRefClass<OwnerType>::value, "The owner of a DependencyProperty must be a ref class");

        return Windows::UI::Xaml::DependencyProperty::Register(
            Platform::StringReference(name),
            ThisDependencyPropertyType::typeid, // Work around bugs in Xaml by using the filtered type
            OwnerType::typeid,
            metadata);
    }

    template <typename TOwner, typename TType>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyProperty(_In_ const wchar_t* const name)
    {
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;

        return RegisterDependencyProperty<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(Details::MakeDefault<ThisPropertyType>()));
    }

    template <typename TOwner, typename TType>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyProperty(_In_ const wchar_t* const name, TType defaultValue)
    {
        return RegisterDependencyProperty<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(defaultValue));
    }

    template <typename TOwner, typename TType, typename TCallback>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyWithCallback(
        _In_ wchar_t const * const name,
        TCallback callback)
    {
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        return RegisterDependencyProperty<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(
                Details::MakeDefault<ThisPropertyType>(),
                ref new Windows::UI::Xaml::PropertyChangedCallback(callback)));
    }

    template <typename TOwner, typename TType, typename TCallback>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyWithCallback(
        _In_ wchar_t const * const name,
        TType defaultValue,
        TCallback callback)
    {
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        return RegisterDependencyProperty<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(
                defaultValue,
                ref new Windows::UI::Xaml::PropertyChangedCallback(callback)));
    }

    // Attached DependencyProperty
    template <typename TOwner, typename TType>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyAttached(
        _In_ const wchar_t* const name,
        _In_ Windows::UI::Xaml::PropertyMetadata^ metadata)
    {
        typedef typename Details::RemoveHat<TOwner>::type OwnerType;
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        typedef typename Details::TypeToUseForDependencyProperty<ThisPropertyType>::type ThisDependencyPropertyType;

        static_assert(Details::IsRefClass<OwnerType>::value, "The owner of a DependencyProperty must be a ref class");

        return Windows::UI::Xaml::DependencyProperty::RegisterAttached(
            Platform::StringReference(name),
            ThisDependencyPropertyType::typeid, // Work around bugs in Xaml by using the filtered type
            OwnerType::typeid,
            metadata);
    }

    template <typename TOwner, typename TType>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyAttached(_In_ const wchar_t* const name)
    {
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        return RegisterDependencyPropertyAttached<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(Details::MakeDefault<ThisPropertyType>()));
    }

    template <typename TOwner, typename TType>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyAttached(_In_ const wchar_t* const name, TType defaultValue)
    {
        return RegisterDependencyPropertyAttached<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(defaultValue));
    }

    template <typename TOwner, typename TType, typename TCallback>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyAttachedWithCallback(
        _In_ wchar_t const * const name,
        TCallback callback)
    {
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        return RegisterDependencyPropertyAttached<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(
                Details::MakeDefault<ThisPropertyType>(),
                ref new Windows::UI::Xaml::PropertyChangedCallback(callback)));
    }

    template <typename TOwner, typename TType, typename TCallback>
    Windows::UI::Xaml::DependencyProperty^ RegisterDependencyPropertyAttachedWithCallback(
        _In_ wchar_t const * const name,
        TType defaultValue,
        TCallback callback)
    {
        typedef typename Details::RemoveHat<TType>::type ThisPropertyType;
        return RegisterDependencyPropertyAttached<TOwner, TType>(
            name,
            ref new Windows::UI::Xaml::PropertyMetadata(
                defaultValue,
                ref new Windows::UI::Xaml::PropertyChangedCallback(callback)));
    }

    void IFTPlatformException(HRESULT hr);
    bool IsLastCharacterTarget(std::wstring const& input, wchar_t target);

    // Return wstring after removing characters specified by unwantedChars array
    template <size_t N>
    std::wstring RemoveUnwantedCharsFromString(std::wstring inputString, const wchar_t (&unwantedChars)[N])
    {
        for (const wchar_t unwantedChar : unwantedChars)
        {
            inputString.erase(std::remove(inputString.begin(), inputString.end(), unwantedChar), inputString.end());
        }
        return inputString;
    }

    double GetDoubleFromWstring(std::wstring input);
    int GetWindowId();
    void RunOnUIThreadNonblocking(std::function<void()>&& function, _In_ Windows::UI::Core::CoreDispatcher ^ currentDispatcher);

    Windows::Foundation::DateTime GetUniversalSystemTime();
    bool IsDateTimeOlderThan(Windows::Foundation::DateTime dateTime, const long long duration);

    concurrency::task<void> WriteFileToFolder(Windows::Storage::IStorageFolder^ folder, Platform::String^ fileName, Platform::String^ contents, Windows::Storage::CreationCollisionOption collisionOption);
    concurrency::task<Platform::String^> ReadFileFromFolder(Windows::Storage::IStorageFolder^ folder, Platform::String^ fileName);

    bool AreColorsEqual(const Windows::UI::Color& color1, const Windows::UI::Color& color2);

    Platform::String^ Trim(Platform::String^ value);
    void Trim(std::wstring& value);
    void TrimFront(std::wstring& value);
    void TrimBack(std::wstring& value);

    Platform::String ^ EscapeHtmlSpecialCharacters(Platform::String ^ originalString, std::shared_ptr<std::vector<wchar_t>> specialCharacters = nullptr);

    Windows::UI::Xaml::Media::SolidColorBrush ^ GetContrastColor(Windows::UI::Color backgroundColor);
}

// This goes into the header to define the property, in the public: section of the class
#define DEPENDENCY_PROPERTY_OWNER(owner)                                                                                                                       \
private:                                                                                                                                                       \
    typedef owner DependencyPropertiesOwner;                                                                                                                   \
                                                                                                                                                               \
public:

// Normal DependencyProperty
#define DEPENDENCY_PROPERTY(type, name)                                                                                                                        \
    property type name                                                                                                                                         \
    {                                                                                                                                                          \
        type get()                                                                                                                                             \
        {                                                                                                                                                      \
            return safe_cast<type>(GetValue(s_##name##Property));                                                                                              \
        }                                                                                                                                                      \
        void set(type value)                                                                                                                                   \
        {                                                                                                                                                      \
            SetValue(s_##name##Property, value);                                                                                                               \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyProperty<DependencyPropertiesOwner, type>(L#name);                                                                     \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_WITH_DEFAULT(type, name, defaultValue)                                                                                             \
    property type name                                                                                                                                         \
    {                                                                                                                                                          \
        type get()                                                                                                                                             \
        {                                                                                                                                                      \
            return safe_cast<type>(GetValue(s_##name##Property));                                                                                              \
        }                                                                                                                                                      \
        void set(type value)                                                                                                                                   \
        {                                                                                                                                                      \
            SetValue(s_##name##Property, value);                                                                                                               \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyProperty<DependencyPropertiesOwner, type>(L#name, defaultValue);                                                       \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_WITH_CALLBACK(type, name)                                                                                                          \
    property type name                                                                                                                                         \
    {                                                                                                                                                          \
        type get()                                                                                                                                             \
        {                                                                                                                                                      \
            return safe_cast<type>(GetValue(s_##name##Property));                                                                                              \
        }                                                                                                                                                      \
        void set(type value)                                                                                                                                   \
        {                                                                                                                                                      \
            SetValue(s_##name##Property, value);                                                                                                               \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyPropertyWithCallback<DependencyPropertiesOwner, type>(L#name, &On##name##PropertyChangedImpl);                         \
    }                                                                                                                                                          \
    static void On##name##PropertyChangedImpl(Windows::UI::Xaml::DependencyObject ^ sender, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args)      \
    {                                                                                                                                                          \
        auto self = safe_cast<DependencyPropertiesOwner ^>(sender);                                                                                            \
        self->On##name##PropertyChanged(safe_cast<type>(args->OldValue), safe_cast<type>(args->NewValue));                                                     \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(type, name, defaultValue)                                                                                \
    property type name                                                                                                                                         \
    {                                                                                                                                                          \
        type get()                                                                                                                                             \
        {                                                                                                                                                      \
            return safe_cast<type>(GetValue(s_##name##Property));                                                                                              \
        }                                                                                                                                                      \
        void set(type value)                                                                                                                                   \
        {                                                                                                                                                      \
            SetValue(s_##name##Property, value);                                                                                                               \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyPropertyWithCallback<DependencyPropertiesOwner, type>(L#name, defaultValue, &On##name##PropertyChangedImpl);           \
    }                                                                                                                                                          \
    static void On##name##PropertyChangedImpl(Windows::UI::Xaml::DependencyObject ^ sender, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args)      \
    {                                                                                                                                                          \
        auto self = safe_cast<DependencyPropertiesOwner ^>(sender);                                                                                            \
        self->On##name##PropertyChanged(safe_cast<type>(args->OldValue), safe_cast<type>(args->NewValue));                                                     \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

// Attached DependencyProperty
#define DEPENDENCY_PROPERTY_ATTACHED(type, name)                                                                                                               \
    static type Get##name(Windows::UI::Xaml::DependencyObject ^ target)                                                                                        \
    {                                                                                                                                                          \
        return safe_cast<type>(target->GetValue(s_##name##Property));                                                                                          \
    }                                                                                                                                                          \
    static void Set##name(Windows::UI::Xaml::DependencyObject ^ target, type value)                                                                            \
    {                                                                                                                                                          \
        target->SetValue(s_##name##Property, value);                                                                                                           \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyPropertyAttached<DependencyPropertiesOwner, type>(L#name);                                                             \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_ATTACHED_WITH_DEFAULT(type, name, defaultValue)                                                                                    \
    static type Get##name(Windows::UI::Xaml::DependencyObject ^ target)                                                                                        \
    {                                                                                                                                                          \
        return safe_cast<type>(target->GetValue(s_##name##Property));                                                                                          \
    }                                                                                                                                                          \
    static void Set##name(Windows::UI::Xaml::DependencyObject ^ target, type value)                                                                            \
    {                                                                                                                                                          \
        target->SetValue(s_##name##Property, value);                                                                                                           \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyPropertyAttached<DependencyPropertiesOwner, type>(L#name, defaultValue);                                               \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(type, name)                                                                                                 \
    static type Get##name(Windows::UI::Xaml::DependencyObject ^ target)                                                                                        \
    {                                                                                                                                                          \
        return safe_cast<type>(target->GetValue(s_##name##Property));                                                                                          \
    }                                                                                                                                                          \
    static void Set##name(Windows::UI::Xaml::DependencyObject ^ target, type value)                                                                            \
    {                                                                                                                                                          \
        target->SetValue(s_##name##Property, value);                                                                                                           \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyPropertyAttachedWithCallback<DependencyPropertiesOwner, type>(L#name, &On##name##PropertyChangedImpl);                 \
    }                                                                                                                                                          \
    static void On##name##PropertyChangedImpl(Windows::UI::Xaml::DependencyObject ^ sender, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args)      \
    {                                                                                                                                                          \
        On##name##PropertyChanged(sender, safe_cast<type>(args->OldValue), safe_cast<type>(args->NewValue));                                                   \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

#define DEPENDENCY_PROPERTY_ATTACHED_WITH_DEFAULT_AND_CALLBACK(type, name, defaultValue)                                                                       \
    static type Get##name(Windows::UI::Xaml::DependencyObject ^ target)                                                                                        \
    {                                                                                                                                                          \
        return safe_cast<type>(target->GetValue(s_##name##Property));                                                                                          \
    }                                                                                                                                                          \
    static void Set##name(Windows::UI::Xaml::DependencyObject ^ target, type value)                                                                            \
    {                                                                                                                                                          \
        target->SetValue(s_##name##Property, value);                                                                                                           \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ s_##name##Property;                                                                                         \
                                                                                                                                                               \
public:                                                                                                                                                        \
    static property Windows::UI::Xaml::DependencyProperty ^ name##Property                                                                                     \
    {                                                                                                                                                          \
        Windows::UI::Xaml::DependencyProperty ^ get() {                                                                                                        \
            assert(s_##name##Property);                                                                                                                        \
            return s_##name##Property;                                                                                                                         \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
                                                                                                                                                               \
private:                                                                                                                                                       \
    static Windows::UI::Xaml::DependencyProperty ^ Initialize##name##Property()                                                                                \
    {                                                                                                                                                          \
        return Utils::RegisterDependencyPropertyAttachedWithCallback<DependencyPropertiesOwner, type>(L#name, defaultValue, &On##name##PropertyChangedImpl);   \
    }                                                                                                                                                          \
    static void On##name##PropertyChangedImpl(Windows::UI::Xaml::DependencyObject ^ sender, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args)      \
    {                                                                                                                                                          \
        On##name##PropertyChanged(sender, safe_cast<type>(args->OldValue), safe_cast<type>(args->NewValue));                                                   \
    }                                                                                                                                                          \
                                                                                                                                                               \
public:

// This goes into the cpp to initialize the static variable
#define DEPENDENCY_PROPERTY_INITIALIZATION(owner, name) Windows::UI::Xaml::DependencyProperty ^ owner::s_##name##Property = owner::Initialize##name##Property();

namespace CalculatorApp
{
    template <typename T>
    T from_cx(Platform::Object ^ from)
    {
        T to{ nullptr };

        winrt::check_hresult(reinterpret_cast<::IUnknown*>(from)->QueryInterface(winrt::guid_of<T>(), reinterpret_cast<void**>(winrt::put_abi(to))));

        return to;
    }
}

// There's no standard definition of equality for Windows::UI::Color structs.
// Define a template specialization for std::equal_to.
template<>
class std::equal_to<Windows::UI::Color>
{
public:
    bool operator()(const Windows::UI::Color& color1, const Windows::UI::Color& color2)
    {
        return Utils::AreColorsEqual(color1, color2);
    }
};

bool operator==(const Windows::UI::Color& color1, const Windows::UI::Color& color2);
bool operator!=(const Windows::UI::Color& color1, const Windows::UI::Color& color2);
