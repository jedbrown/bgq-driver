/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2009, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/********************************************************************/
#include <Python.h>
#include <bgsched/bgsched.h>
#include <bgsched/realtime/Client.h>
#include <bgsched/realtime/ClientConfiguration.h>
#include <bgsched/realtime/Filter.h>
#include <bgsched/realtime/ClientEventListener.h>
#include <bgsched/realtime/types.h>
#include <string>
#include <iostream>
#include <utility/include/Log.h>
#include <utility/include/Properties.h>

using namespace std;
LOG_DECLARE_FILE("teal");

static PyObject *py_self = NULL;
static PyObject *py_callback = NULL;
static PyObject *py_term_callback = NULL;

void ras_event_term_callback()
{
  LOG_TRACE_MSG("ras_event_term_callback is called");

  // restore the python thread state and get the global lock
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  // call the python callfor RealtimeEventMonitor
  PyObject *result = PyEval_CallObject(py_term_callback, NULL);
  if (result != NULL)
    Py_DECREF(result);

  // release the python global lock
  PyGILState_Release(gstate);
  
  return;
}

class RasEventListener: public bgsched::realtime::ClientEventListener 
{
public:
  RasEventListener() { }
  
  void handleRealtimeStartedRealtimeEvent(const bgsched::realtime::ClientEventListener::RealtimeStartedEventInfo& info ) {
  }

  void handleRasRealtimeEvent( const bgsched::realtime::ClientEventListener::RasEventInfo& info) {

    bgsched::realtime::RasRecordId recid = info.getRecordId();
    string msgid = info.getMessageId();
    LOG_TRACE_MSG("Handle Ras event with recid " << recid);

    // restore the python thread state and get the global lock
    PyGILState_STATE gstate = PyGILState_Ensure();

    // call the python callback for RealtimeEventMonitor
    PyObject *arglist = Py_BuildValue("(Is)", recid, (char *) msgid.c_str()); 
    PyObject *result = PyEval_CallObject(py_callback, arglist);
    Py_DECREF(arglist);
    if (result != NULL)
      Py_DECREF(result);

    // release the python global lock
    PyGILState_Release(gstate);

    LOG_TRACE_MSG("Exit handle Ras event");
  }

  bool getRealtimeContinue() { 
      LOG_TRACE_MSG("Set indicator to continue receiving RAS events from real-time server");
      return true;
  }
};


int ras_event_poller(char *msgid_filter) {

  LOG_TRACE_MSG("enter ras_event_poller");

  // Initialize logger and database
  string properties;
  bgsched::init(properties);

  const bgsched::realtime::ClientConfiguration& config = 
    bgsched::realtime::ClientConfiguration::getDefault();

  bgsched::realtime::Filter filter = bgsched::realtime::Filter::createNone();
  filter.setRasEvents(true);
  const string msgids(msgid_filter);
  filter.setRasMessageIdPattern(&msgids);

  bgsched::realtime::Client client(config);
  bgsched::realtime::Filter::Id filterId;
  client.setFilter(filter, &filterId, NULL);

  RasEventListener listener = RasEventListener();
  client.addListener(listener);
  client.setBlocking(true);

  try {
    LOG_TRACE_MSG("Connecting to real-time server");
    client.connect();
    LOG_TRACE_MSG("Connection successful");

    // Request Ras events update from real-time server
    client.requestUpdates(NULL);

    // The end indicator is set to true if real-time server closes the connection
    bool end = false;
    client.receiveMessages(NULL, NULL, &end);

  } catch (exception &e) {
    LOG_ERROR_MSG(e.what());  
    return 0;
  }

  LOG_TRACE_MSG("exit ras_event_poller");
  return 0;
}

static PyObject * 
ras_init(PyObject *self, PyObject *args)
{
  //  Initialize logging
  const bgq::utility::Properties::ConstPtr properties(
          bgq::utility::Properties::create()
          );
  bgq::utility::initializeLogging(*properties);

  // Set the python callback for RealtimeEventMonitor
  LOG_DEBUG_MSG("enter ras_init");
  py_self = self;
  char *filter;
  PyObject *cb, *term_cb;

  if (PyArg_ParseTuple(args, "sOO:set_callback", &filter, &cb, &term_cb)) {
    if (!PyCallable_Check(cb)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    if (!PyCallable_Check(term_cb)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    Py_XINCREF(cb);           /* Add a reference to new callback */
    Py_XDECREF(py_callback);  /* Dispose of previous callback */
    py_callback = cb  ;       /* Remember new callback */
    Py_XINCREF(term_cb);           /* Add a reference to new callback */
    Py_XDECREF(py_term_callback);  /* Dispose of previous callback */
    py_term_callback = term_cb  ;  /* Remember new callback */
  }

  // Release the python global lock and call the BG Realtime setup 
  int rc;
  Py_BEGIN_ALLOW_THREADS
  rc = ras_event_poller(filter);
  ras_event_term_callback(); 
  Py_END_ALLOW_THREADS
  PyObject *arg = Py_BuildValue("i", rc);
  return arg;
}

/* registration table */
static struct PyMethodDef pyrealtime_methods[] = {
  {"ras_init",ras_init,1},  // method name, C func ptr
  {NULL,NULL}         // end of table 
};

static PyObject *PyRealtimeError;

/* module initializer */
PyMODINIT_FUNC
initpyrealtime()
{
  PyObject *m = Py_InitModule("pyrealtime", pyrealtime_methods);

  PyRealtimeError = PyErr_NewException("pyrealtime.error", NULL, NULL);
  Py_INCREF(PyRealtimeError);
  PyModule_AddObject(m, "error", PyRealtimeError);
}
