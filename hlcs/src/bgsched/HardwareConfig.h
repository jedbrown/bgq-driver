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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

/*!
 * \file bgsched/HardwareConfig.h
 * \brief HardwareConfig class definition
 */

#ifndef BGSCHED_HARDWARE_CONFIG_H_
#define BGSCHED_HARDWARE_CONFIG_H_

namespace bgsched {

/*!
 * \brief Represents configuration parameters for hardware.
 */
class HardwareConfig
{
public:

    /*!
     * \brief
     */
    HardwareConfig();

    /*!
     * \brief Get indication if using database for storing compute hardware info.
     *
     * \return True if no database is being used, false if database is being used.
     */
    bool getNoDatabase() const;

    /*!
     * \brief Get machine rows.
     *
     * \return Machine rows.
     */
    uint32_t getRows() const;

    /*!
     * \brief Get machine columns.
     *
     * \return Machine columns.
     */
    uint32_t getColumns() const;

    /*!
     * \brief Set machine rows.
     */
    void setRows(
            const uint32_t rows     //!< [in] Number of rows
    );

    /*!
     * \brief Set machine columns.
     */
    void setColumns(
            const uint32_t columns  //!< [in] Number of columns
    );

protected:

    bool     _noDb;     //!< true if hardware information should NOT be read from the database
    uint32_t _rows;     //!< if _noDB is true, the number of rows to construct for the machine
    uint32_t _columns;  //!< if _noDB is true, the number of rows to construct for the machine

};

} // namespace bgsched

#endif
