// generated by ./ddl2cpp sql_files.sql sql_files tbls
#ifndef TBLS_SQL_FILES_H
#define TBLS_SQL_FILES_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace tbls
{
  namespace TabInodes_
  {
    struct Inode
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "inode";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T inode;
            T& operator()() { return inode; }
            const T& operator()() const { return inode; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::can_be_null>;
    };
    struct Size
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "size";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T size;
            T& operator()() { return size; }
            const T& operator()() const { return size; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
    struct Hash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "hash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T hash;
            T& operator()() { return hash; }
            const T& operator()() const { return hash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::blob, sqlpp::tag::can_be_null>;
    };
    struct Mtime
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "mtime";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T mtime;
            T& operator()() { return mtime; }
            const T& operator()() const { return mtime; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
    };
    struct LastCheck
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "last_check";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lastCheck;
            T& operator()() { return lastCheck; }
            const T& operator()() const { return lastCheck; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
    };
    struct Ack
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ack";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ack;
            T& operator()() { return ack; }
            const T& operator()() const { return ack; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::can_be_null>;
    };
  }

  struct TabInodes: sqlpp::table_t<TabInodes,
               TabInodes_::Inode,
               TabInodes_::Size,
               TabInodes_::Hash,
               TabInodes_::Mtime,
               TabInodes_::LastCheck,
               TabInodes_::Ack>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tab_inodes";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tabInodes;
        T& operator()() { return tabInodes; }
        const T& operator()() const { return tabInodes; }
      };
    };
  };
  namespace TabDentries_
  {
    struct Dnode
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "dnode";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T dnode;
            T& operator()() { return dnode; }
            const T& operator()() const { return dnode; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
    struct Name
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Inode
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "inode";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T inode;
            T& operator()() { return inode; }
            const T& operator()() const { return inode; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
    struct Ack
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ack";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ack;
            T& operator()() { return ack; }
            const T& operator()() const { return ack; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::can_be_null>;
    };
  }

  struct TabDentries: sqlpp::table_t<TabDentries,
               TabDentries_::Dnode,
               TabDentries_::Name,
               TabDentries_::Inode,
               TabDentries_::Ack>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tab_dentries";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tabDentries;
        T& operator()() { return tabDentries; }
        const T& operator()() const { return tabDentries; }
      };
    };
  };
}
#endif
