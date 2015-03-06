/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file api/extension/c/template_extension/Extension.h
 * \brief PAMI "template" extension interface template specialization
 *
 * \todo Update doxygen and preprocessor directives
 */
#ifndef __api_extension_c_template_extension_Extension_h__
#define __api_extension_c_template_extension_Extension_h__
///
/// \page howto_add_ext How to create a new PAMI extension
///
/// 1. Establish a name for the extension, in this example "sample_extension"
/// is used. This name will also be used in the build procedure to enable the
/// extension. See particular platform build instructions.
///
/// For BlueGene, or any platform that uses a configure script, the command line
/// option would be:
///
///	--with-pami-extension=sample_extension[,...]
///
/// For PE, the ADE build would be modified as TBD.
///
/// This name also provides a (base-)name for sub-directories and symbols.
/// Note, this name also forms a #define (-D__pami_extension_<name>__=1)
/// which is generated each platform's build procedure (configure scripts, etc).
/// By convention, the name does not usually include the word "extension".
///
/// 2. Next, obtain a unique number for this extension by examining
/// \b api/extension/registry.def for an un-used number ("####" throughout this
/// file). Also create an entry for this extension there. Note, in that file
/// the extension name is prefixed with a string when used in the macro, e.g.
///
///	PAMI_EXTENSION_DEFINE(EXT_sample_extension,####,0,0)
///
/// This full string (prefix and name) is used when opening the extension
/// from a program or client, for example:
///
///	rc = PAMI_Extension_open("EXT_sample_extension", &myext);
///
/// The prefix has special meaning, e.g.
///
///	EXT_	Official, platform-neutral, extensions
///	BG_	Blue Gene only extensions
///	BGQ_	BG/Q only extensions
///	PE_	PE only extensions
///	HFI_	PERCS only extensions
///	(more?)
///
///	\todo These prefix conventions will be enumerated in registry.h
///
/// Extensions may start out for a single platform only (e.g. BGQ_) and
/// then later be promoted to EXT_. In order to avoid breaking users of the
/// extension, two lines may be created in registry.def for the extension,
/// one with the old prefix and one with EXT_ both with the same ####. In this
/// way, new and old users of the extension will work until old references
/// can be converted.
///
/// 3. Edit the file \b api/extension/registry.h and add an #include to your
/// top-level extension header, for example:
///
///	#include "api/extension/c/sample_extension/Extension.h"
///
/// Note the above use of the extension name as the sub-directory.
///
/// 4. Create this Extension.h file, in your extension sub-directory (this is
/// the file included from registry.h), to contain the common extension interface
/// code, based on what is in the file:
///
///	api/extension/c/template_extension/Extension.h
///
/// This establishes a class name for the extension, typically <name>Extension.
/// Note, your openExtension() method may need to allocate memory if the extension
/// requires non-static members or data (and closeExtension() may then need to free
/// memory). This file must also include an #ifdef of a symbol (with matching
/// #endif), used to control whether an extension is available, of the form
/// "__pami_extension_<name>__". For example:
///
///	#ifdef __pami_extension_sample_extension__
///
/// This #ifdef/#endif must surround the body contents of Extension.h, such that
/// no class or other objects are defined for this extension unless it is defined.
/// The platform build method (configure script, etc) generates compiler directives
/// to define this symbol when the extension has been configured.
///
/// 5. Create a <name>Extension.h file (e.g. SampleExtension.h), along side the
/// Extension.h file created previously, containing interface definitions for
/// this specific extension. This will establish what data members and methods
/// are available, as well as internal structures. This file is only included by
/// your "Extension.h" and implementation (i.e. <name>Extension.cc) file(s).
///
/// 6. If your extension has multiple implementations for different platforms (or
/// some other selection criteria), then create subdirectories for each platform
/// (selection name). Otherwise the implemention file (.cc) may be placed in the
/// extension sub-directory (e.g. sample_extension). Create GNUMakefile.in's as
/// needed for the directory hierarchy. The LIBNAME/DEVICENAME make variables
/// should employ some derivation of the extension name, and must be unique. This
/// also establishes the name of the implementation files (.cc). For example:
///
///	@LIBNAME@-""-SAMPLE_EXTENSION_SOURCES=SampleExtension.cc
///	DEVICENAME = @LIBNAME@-""-SAMPLE_EXTENSION
///
/// 7. Now create the implementation file(s) as needed. These typically instantiate
/// the public methods and data members, as well as set up anything required.
/// Don't forget a constructor for the extension, e.g.
///
///	PAMI::SampleExtension::SampleExtension() {...}
///
/// Summary: The following new files/directories would be created for a
/// platform-neutral extension named "sample_extension":
///
///	api/extension/c/sample_extension
///	api/extension/c/sample_extension/Extension.h
///	api/extension/c/sample_extension/SampleExtension.h
///	api/extension/c/sample_extension/SampleExtension.cc
///	api/extension/c/sample_extension/GNUMakefile.in
///
/// and (only) the following existing files would be modified:
///
///	api/extension/registry.def
///	api/extension/registry.h
///

#ifdef __pami_extension_template_extension__ // configure --with-pami-extension=template_extension

#include "api/extension/Extension.h"

namespace PAMI
{
  ///
  /// \brief Private class used by the template extension
  ///
  /// \note It is not neccesary for an extension to define or use a private
  ///       extension class
  ///
  class ExtensionTemplate
  {
    public:

      /// \brief An arbitrary function that does nothing
      static void foo () {};

      /// \brief Another arbitrary function that does nothing
      static void bar () {};
  };

  ///
  /// \brief Open the extension for use by a client
  ///
  /// An extension cookie is returned after the extension is opened which is
  /// provided as an input parameter when the extension is closed.
  ///
  /// Each extension implementation may define the extension cookie differently.
  /// Often the cookie is a pointer to allocated memory which may contain an
  /// instance of a class that provides the extension capabilities. Any defined
  /// extension classes are private to the extension and are not visible to
  /// objects external to the extension implementation.
  ///
  /// \todo Replace the template parameter with the unique extension identifier
  ///       assigned to the specific extension
  ///
  template <>
  void * Extension::openExtension<####> (pami_client_t   client,
                                         const char    * name,
                                         pami_result_t & result)
  {
    result = PAMI_UNIMPL;
    return NULL;
  }

  ///
  /// \brief Close the extension for use by a client
  ///
  /// The extension cookie was originally provided when the extension was
  /// opened.
  ///
  /// \todo Replace the template parameter with the unique extension identifier
  ///       assigned to the specific extension
  ///
  template <>
  void Extension::closeExtension<####> (void * cookie, pami_result_t & result)
  {
    result = PAMI_UNIMPL;
    return;
  }

  ///
  /// \brief Query the extension for a named function
  ///
  /// Returns a function pointer to the static function associated with the
  /// input parameter name.
  ///
  /// \todo Replace the template parameter with the unique extension identifier
  ///       assigned to the specific extension
  ///
  template <>
  void * Extension::queryExtension<####> (const char * name, void * cookie)
  {
    if (strcasecmp (name, "foo") == 0)
      return (void *) PAMI::ExtensionTest::foo;

    if (strcasecmp (name, "bar") == 0)
      return (void *) PAMI::ExtensionTest::bar;

    return NULL;
  };
};

#endif // __pami_extension_template_extension__
#endif // __api_extension_c_template_extension_Extension_h__
