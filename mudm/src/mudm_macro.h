/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */


#ifndef	_MUDM_MACRO_H_ /* Prevent multiple inclusion */
#define	_MUDM_MACRO_H_

#ifndef __LINUX__ 
#ifndef likely
#define likely(x) (__builtin_expect(x,1))
#endif
#ifndef unlikely
#define unlikely(x) (__builtin_expect(x,0)) 
#endif 
#endif

#define LLUS long long unsigned int



//Will print flight recorder data  !!!!!!!!!!!!!
//#define PRINT_FLIGHT_LOGGING 1

//MUST PRINT
#ifdef __LINUX__ 
#define MPRINT(format, args...) printk(KERN_EMERG format, ##args)
#else
#define MPRINT(format, args...) printf(format, ##args)
#endif
#define MDUMPHEXSTR(str,ptr,len) MPRINT("%s for %p bytes=%d \n",str,ptr,(int)len);    mudm_dumphex( (unsigned char *)ptr, (int)len);

#define MENTER MPRINT("->%s():%d\n", __FUNCTION__, __LINE__);
#define MEXIT MPRINT("<-%s():%d\n", __FUNCTION__, __LINE__);
#define MHERE MPRINT("@ %s():%d\n", __FUNCTION__, __LINE__);


//Using MUDM_PRINT define here will globally print.  Very Noisy!!!!!!!!!!!!!!!!!!!!!!!
//#define MUDM_PRINT 1



/*control what prints out */
#ifdef __LINUX__ 


#ifndef MUDM_PRINT
#define DPRINT(format, args...) 
#define PRINT(format, args...) 
#define CPRINT(format, args...) 
#else
#define DPRINT(format, args...) printk(KERN_EMERG format, ##args)
#define PRINT(format, args...) printk(KERN_EMERG format, ##args)
#define CPRINT(format, args...) printk(KERN_EMERG format, ##args)
#endif

#else
#ifndef MUDM_PRINT
#define DPRINT(format, args...) 
#define PRINT(format, args...) 
#define CPRINT(format, args...)
#else
#define DPRINT(format, args...) printf(format, ##args)
#define PRINT(format, args...) printf(format, ##args)
#define CPRINT(format, args...) printf(format, ##args)
#endif 

#endif //ifdef LINUX else

#ifndef MUDM_PRINT
#define DUMPHEX(ptr,len)
#define DUMPHEXSTR(str,ptr,len)
#else
#ifndef PRINT_FLIGHT_LOGGING
#define PRINT_FLIGHT_LOGGING 1
#endif
#define DUMPHEX(ptr,len) mudm_dumphex( (unsigned char *)ptr, (int)len)
#define DUMPHEXSTR(str,ptr,len) PRINT("%s for %p \n",str,ptr);    mudm_dumphex( (unsigned char *)ptr, (int)len);
#endif


#ifdef DEBUG
#define ENTER PRINT("->%s():%d\n", __FUNCTION__, __LINE__);
#define EXIT PRINT("<-%s():%d\n", __FUNCTION__, __LINE__);
#define HERE PRINT("@ %s():%d\n", __FUNCTION__, __LINE__);
#else
#define ENTER
#define EXIT
#define HERE
#endif


#endif //_MUDM_MACRO_H_
