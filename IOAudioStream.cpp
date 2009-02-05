/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#ifdef DEBUG
#define DEBUG_OUTPUT
#endif

#include <IOKit/audio/IOAudioStream.h>
#include <IOKit/audio/IOAudioEngine.h>
#include <IOKit/audio/IOAudioEngineUserClient.h>
#include <IOKit/audio/IOAudioControl.h>
#include <IOKit/audio/IOAudioTypes.h>
#include <IOKit/audio/IOAudioDefines.h>
#include <IOKit/audio/IOAudioDebug.h>

#include <IOKit/IOLib.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOCommandGate.h>

#include <libkern/c++/OSSymbol.h>
#include <libkern/c++/OSNumber.h>
#include <libkern/c++/OSArray.h>
#include <libkern/c++/OSDictionary.h>

typedef struct IOAudioStreamFormatDesc {
    IOAudioStreamFormat				format;
    IOAudioSampleRate				minimumSampleRate;
    IOAudioSampleRate				maximumSampleRate;
    IOAudioStream::AudioIOFunction	*ioFunctionList;
    UInt32							numIOFunctions;
} IOAudioStreamFormatDesc;

#define super IOService
OSDefineMetaClassAndStructors(IOAudioStream, IOService)
OSMetaClassDefineReservedUnused(IOAudioStream, 0);
OSMetaClassDefineReservedUnused(IOAudioStream, 1);
OSMetaClassDefineReservedUnused(IOAudioStream, 2);
OSMetaClassDefineReservedUnused(IOAudioStream, 3);
OSMetaClassDefineReservedUnused(IOAudioStream, 4);
OSMetaClassDefineReservedUnused(IOAudioStream, 5);
OSMetaClassDefineReservedUnused(IOAudioStream, 6);
OSMetaClassDefineReservedUnused(IOAudioStream, 7);
OSMetaClassDefineReservedUnused(IOAudioStream, 8);
OSMetaClassDefineReservedUnused(IOAudioStream, 9);
OSMetaClassDefineReservedUnused(IOAudioStream, 10);
OSMetaClassDefineReservedUnused(IOAudioStream, 11);
OSMetaClassDefineReservedUnused(IOAudioStream, 12);
OSMetaClassDefineReservedUnused(IOAudioStream, 13);
OSMetaClassDefineReservedUnused(IOAudioStream, 14);
OSMetaClassDefineReservedUnused(IOAudioStream, 15);
OSMetaClassDefineReservedUnused(IOAudioStream, 16);
OSMetaClassDefineReservedUnused(IOAudioStream, 17);
OSMetaClassDefineReservedUnused(IOAudioStream, 18);
OSMetaClassDefineReservedUnused(IOAudioStream, 19);
OSMetaClassDefineReservedUnused(IOAudioStream, 20);
OSMetaClassDefineReservedUnused(IOAudioStream, 21);
OSMetaClassDefineReservedUnused(IOAudioStream, 22);
OSMetaClassDefineReservedUnused(IOAudioStream, 23);
OSMetaClassDefineReservedUnused(IOAudioStream, 24);
OSMetaClassDefineReservedUnused(IOAudioStream, 25);
OSMetaClassDefineReservedUnused(IOAudioStream, 26);
OSMetaClassDefineReservedUnused(IOAudioStream, 27);
OSMetaClassDefineReservedUnused(IOAudioStream, 28);
OSMetaClassDefineReservedUnused(IOAudioStream, 29);
OSMetaClassDefineReservedUnused(IOAudioStream, 30);
OSMetaClassDefineReservedUnused(IOAudioStream, 31);
OSMetaClassDefineReservedUnused(IOAudioStream, 32);
OSMetaClassDefineReservedUnused(IOAudioStream, 33);
OSMetaClassDefineReservedUnused(IOAudioStream, 34);
OSMetaClassDefineReservedUnused(IOAudioStream, 35);
OSMetaClassDefineReservedUnused(IOAudioStream, 36);
OSMetaClassDefineReservedUnused(IOAudioStream, 37);
OSMetaClassDefineReservedUnused(IOAudioStream, 38);
OSMetaClassDefineReservedUnused(IOAudioStream, 39);
OSMetaClassDefineReservedUnused(IOAudioStream, 40);
OSMetaClassDefineReservedUnused(IOAudioStream, 41);
OSMetaClassDefineReservedUnused(IOAudioStream, 42);
OSMetaClassDefineReservedUnused(IOAudioStream, 43);
OSMetaClassDefineReservedUnused(IOAudioStream, 44);
OSMetaClassDefineReservedUnused(IOAudioStream, 45);
OSMetaClassDefineReservedUnused(IOAudioStream, 46);
OSMetaClassDefineReservedUnused(IOAudioStream, 47);

const OSSymbol *IOAudioStream::gDirectionKey = NULL;
const OSSymbol *IOAudioStream::gNumChannelsKey = NULL;
const OSSymbol *IOAudioStream::gSampleFormatKey = NULL;
const OSSymbol *IOAudioStream::gNumericRepresentationKey = NULL;
const OSSymbol *IOAudioStream::gBitDepthKey = NULL;
const OSSymbol *IOAudioStream::gBitWidthKey = NULL;
const OSSymbol *IOAudioStream::gAlignmentKey = NULL;
const OSSymbol *IOAudioStream::gByteOrderKey = NULL;
const OSSymbol *IOAudioStream::gIsMixableKey = NULL;
const OSSymbol *IOAudioStream::gDriverTagKey = NULL;
const OSSymbol *IOAudioStream::gMinimumSampleRateKey = NULL;
const OSSymbol *IOAudioStream::gMaximumSampleRateKey = NULL;

void IOAudioStream::initKeys()
{
    if (!gNumChannelsKey) {
        gNumChannelsKey = OSSymbol::withCString(kIOAudioStreamNumChannelsKey);
        gSampleFormatKey = OSSymbol::withCString(kIOAudioStreamSampleFormatKey);
        gNumericRepresentationKey = OSSymbol::withCString(kIOAudioStreamNumericRepresentationKey);
        gBitDepthKey = OSSymbol::withCString(kIOAudioStreamBitDepthKey);
        gBitWidthKey = OSSymbol::withCString(kIOAudioStreamBitWidthKey);
        gAlignmentKey = OSSymbol::withCString(kIOAudioStreamAlignmentKey);
        gByteOrderKey = OSSymbol::withCString(kIOAudioStreamByteOrderKey);
        gIsMixableKey = OSSymbol::withCString(kIOAudioStreamIsMixableKey);
        gDriverTagKey = OSSymbol::withCString(kIOAudioStreamDriverTagKey);

        gDirectionKey = OSSymbol::withCString(kIOAudioStreamDirectionKey);
        
        gMinimumSampleRateKey = OSSymbol::withCString(kIOAudioStreamMinimumSampleRateKey);
        gMaximumSampleRateKey = OSSymbol::withCString(kIOAudioStreamMaximumSampleRateKey);
    }
}

OSDictionary *IOAudioStream::createDictionaryFromFormat(const IOAudioStreamFormat *streamFormat, OSDictionary *formatDict = 0)
{
    OSDictionary *newDict = NULL;

    if (streamFormat) {
        if (formatDict) {
            newDict = formatDict;
        } else {
            newDict = OSDictionary::withCapacity(7);
        }

        if (newDict) {
            OSNumber *num;

            if (!gNumChannelsKey) {
                initKeys();
            }
            
            num = OSNumber::withNumber(streamFormat->fNumChannels, 32);
            newDict->setObject(gNumChannelsKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fSampleFormat, 32);
            newDict->setObject(gSampleFormatKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fNumericRepresentation, 32);
            newDict->setObject(gNumericRepresentationKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fBitDepth, 8);
            newDict->setObject(gBitDepthKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fBitWidth, 8);
            newDict->setObject(gBitWidthKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fAlignment, 8);
            newDict->setObject(gAlignmentKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fByteOrder, 8);
            newDict->setObject(gByteOrderKey, num);
            num->release();

            num = OSNumber::withNumber(streamFormat->fIsMixable, 8);
            newDict->setObject(gIsMixableKey, num);
            num->release();
            
            num = OSNumber::withNumber(streamFormat->fDriverTag, 32);
            newDict->setObject(gDriverTagKey, num);
            num->release();
        }
    }


    return newDict;
}

IOAudioStreamFormat *IOAudioStream::createFormatFromDictionary(const OSDictionary *formatDict, IOAudioStreamFormat *streamFormat = 0)
{
    IOAudioStreamFormat *format = NULL;
    static IOAudioStreamFormat staticFormat;

    if (formatDict) {
        if (streamFormat) {
            format = streamFormat;
        } else {
            format = &staticFormat;
        }

        if (format) {
            OSNumber *num;

            if (!gNumChannelsKey) {
                initKeys();
            }

            bzero(format, sizeof(IOAudioStreamFormat));

            num = OSDynamicCast(OSNumber, formatDict->getObject(gNumChannelsKey));
            if (num) {
                format->fNumChannels = num->unsigned32BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gSampleFormatKey));
            if (num) {
                format->fSampleFormat = num->unsigned32BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gNumericRepresentationKey));
            if (num) {
                format->fNumericRepresentation = num->unsigned32BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gBitDepthKey));
            if (num) {
                format->fBitDepth = num->unsigned8BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gBitWidthKey));
            if (num) {
                format->fBitWidth = num->unsigned8BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gAlignmentKey));
            if (num) {
                format->fAlignment = num->unsigned8BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gByteOrderKey));
            if (num) {
                format->fByteOrder = num->unsigned8BitValue();
            }

            num = OSDynamicCast(OSNumber, formatDict->getObject(gIsMixableKey));
            if (num) {
                format->fIsMixable = num->unsigned8BitValue();
            }
            
            num = OSDynamicCast(OSNumber, formatDict->getObject(gDriverTagKey));
            if (num) {
                format->fDriverTag = num->unsigned32BitValue();
            }
        }
    }

    return format;
}


bool IOAudioStream::initWithAudioEngine(IOAudioEngine *engine, IOAudioStreamDirection dir, UInt32 startChannelID, const char *streamDescription, OSDictionary *properties)
{
    if (!gNumChannelsKey) {
        initKeys();
    }

    if (!engine) {
        return false;
    }

    if (!super::init(properties)) {
        return false;
    }

    audioEngine = engine;
    
    workLoop = audioEngine->getWorkLoop();
    if (!workLoop) {
        return false;
    }

    workLoop->retain();
    
    commandGate = IOCommandGate::commandGate(this);
    if (!commandGate) {
        return false;
    }
    
    streamIOLock = IORecursiveLockAlloc();
    if (!streamIOLock) {
        return false;
    }
    
    setDirection(dir);
    
    startingChannelID = startChannelID;
    setProperty(kIOAudioStreamStartingChannelIDKey, startingChannelID, sizeof(UInt32)*8);
    
    maxNumChannels = 0;
    
    if (streamDescription) {
        setProperty(kIOAudioStreamDescriptionKey, streamDescription);
    }
    
    availableFormatDictionaries = OSArray::withCapacity(1);
    if (!availableFormatDictionaries) {
        return false;
    }
    setProperty(kIOAudioStreamAvailableFormatsKey, availableFormatDictionaries);
    
    setProperty(kIOAudioStreamIDKey, (UInt32)this, sizeof(UInt32)*8);
    
    streamAvailable = true;
    setProperty(kIOAudioStreamAvailableKey, (UInt8)1, sizeof(UInt8)*8);
    
    numClients = 0;
    updateNumClients();
    
    resetClipInfo();
    
    clientBufferListStart = NULL;
    clientBufferListEnd = NULL;
    
    userClientList = NULL;
    
    audioIOFunctions = NULL;
    numIOFunctions = false;
    
    streamAllocatedMixBuffer = false;

    workLoop->addEventSource(commandGate);

    return true;
}

void IOAudioStream::free()
{
    if (availableFormatDictionaries) {
        availableFormatDictionaries->release();
        availableFormatDictionaries = NULL;
    }
    
    if (mixBuffer && streamAllocatedMixBuffer) {
        IOFreeAligned(mixBuffer, mixBufferSize);
        mixBuffer = NULL;
        mixBufferSize = 0;
    }
    
    if (defaultAudioControls) {
        removeDefaultAudioControls();
        defaultAudioControls->release();
        defaultAudioControls = NULL;
    }

    if (commandGate) {
        if (workLoop) {
            workLoop->removeEventSource(commandGate);
        }

        commandGate->release();
        commandGate = NULL;
    }

    if (workLoop) {
        workLoop->release();
        workLoop = NULL;
    }
    
    if (streamIOLock) {
        IORecursiveLockFree(streamIOLock);
        streamIOLock = NULL;
    }
    
    if (audioIOFunctions && (numIOFunctions > 0)) {
        IOFreeAligned(audioIOFunctions, numIOFunctions * sizeof(AudioIOFunction));
        audioIOFunctions = NULL;
        numIOFunctions = 0;
    }
    
    if (availableFormats && (numAvailableFormats > 0)) {
        UInt32 formatNum;
        
        for (formatNum = 0; formatNum < numAvailableFormats; formatNum++) {
            if (availableFormats[formatNum].ioFunctionList && (availableFormats[formatNum].numIOFunctions > 0)) {
                IOFreeAligned(availableFormats[formatNum].ioFunctionList, availableFormats[formatNum].numIOFunctions * sizeof(AudioIOFunction));
            }
        }
        
        IOFreeAligned(availableFormats, numAvailableFormats * sizeof(IOAudioStreamFormatDesc));
        availableFormats = NULL;
        numAvailableFormats = 0;
    }

    super::free();
}

void IOAudioStream::stop(IOService *provider)
{
    if (commandGate) {
        if (workLoop) {
            workLoop->removeEventSource(commandGate);
        }
        
        commandGate->release();
        commandGate = NULL;
    }
    
    super::stop(provider);
}

IOWorkLoop *IOAudioStream::getWorkLoop() const
{	
    return workLoop;
}

IOReturn IOAudioStream::setProperties(OSObject *properties)
{
    OSDictionary *props;
    IOReturn result = kIOReturnSuccess;
    
#ifdef DEBUG_CALLS
    IOLog("IOAudioStream[%p]::setProperties(%p)\n", this, properties);
#endif

    if (properties && (props = OSDynamicCast(OSDictionary, properties))) {
        OSCollectionIterator *iterator;
        OSObject *iteratorKey;

        iterator = OSCollectionIterator::withCollection(props);
        if (iterator) {
            while (iteratorKey = iterator->getNextObject()) {
                OSSymbol *key;

                key = OSDynamicCast(OSSymbol, iteratorKey);
                if (key && key->isEqualTo(kIOAudioStreamFormatKey)) {
                    OSDictionary *formatDict = OSDynamicCast(OSDictionary, props->getObject(key));
                    if (formatDict) {
                        assert(commandGate);
                        result = commandGate->runAction(setFormatAction, formatDict);
                    }
                }
            }
            iterator->release();
        } else {
            result = kIOReturnError;
        }
    } else {
        result = kIOReturnBadArgument;
    }

    return result;
}

void IOAudioStream::setDirection(IOAudioStreamDirection dir)
{
    direction = dir;
    setProperty(kIOAudioStreamDirectionKey, direction, 8);
}

IOAudioStreamDirection IOAudioStream::getDirection()
{
    return direction;
}

void IOAudioStream::setSampleBuffer(void *buffer, UInt32 size)
{
    lockStreamForIO();
    
    sampleBuffer = buffer;

    if (sampleBuffer) {
        sampleBufferSize = size;
        bzero(sampleBuffer, sampleBufferSize);
    } else {
        sampleBufferSize = 0;
    }
    
    unlockStreamForIO();
}

void *IOAudioStream::getSampleBuffer()
{
    return sampleBuffer;
}

UInt32 IOAudioStream::getSampleBufferSize()
{
    return sampleBufferSize;
}

void IOAudioStream::setMixBuffer(void *buffer, UInt32 size)
{
    lockStreamForIO();
      
    if (mixBuffer && streamAllocatedMixBuffer) {
        IOFreeAligned(mixBuffer, mixBufferSize);
        mixBuffer = NULL;
        mixBufferSize = 0;
        streamAllocatedMixBuffer = false;
    }
    
    mixBuffer = buffer;

    if (mixBuffer) {
        mixBufferSize = size;
        bzero(mixBuffer, mixBufferSize);
    } else {
        mixBufferSize = 0;
    }
    
    unlockStreamForIO();
}

void *IOAudioStream::getMixBuffer()
{
    return mixBuffer;
}

UInt32 IOAudioStream::getMixBufferSize()
{
    return mixBufferSize;
}

void IOAudioStream::numSampleFramesPerBufferChanged()
{
    if (mixBuffer && streamAllocatedMixBuffer) {
        setMixBuffer(NULL, 0);
    }
}

void IOAudioStream::clearSampleBuffer()
{
    if (sampleBuffer && (sampleBufferSize > 0)) {
        bzero(sampleBuffer, sampleBufferSize);
    }
    
    if (mixBuffer && (mixBufferSize > 0)) {
        bzero(mixBuffer, mixBufferSize);
    }
}

void IOAudioStream::setIOFunction(AudioIOFunction ioFunction)
{
    setIOFunctionList(&ioFunction, 1);
}

void IOAudioStream::setIOFunctionList(const AudioIOFunction *ioFunctionList, UInt32 numFunctions)
{
    lockStreamForIO();

    if (audioIOFunctions && (numIOFunctions > 0)) {
        IOFreeAligned(audioIOFunctions, numIOFunctions * sizeof(AudioIOFunction));
        audioIOFunctions = NULL;
        numIOFunctions = 0;
    }
    
    if (ioFunctionList && (numFunctions != 0)) {
        audioIOFunctions = (AudioIOFunction *)IOMallocAligned(numFunctions * sizeof(AudioIOFunction), sizeof (AudioIOFunction *));
        if (audioIOFunctions) {
            memcpy(audioIOFunctions, ioFunctionList, numFunctions * sizeof(AudioIOFunction));
            numIOFunctions = numFunctions;
        }
    }

    unlockStreamForIO();
}

const IOAudioStreamFormat *IOAudioStream::getFormat()
{
    return &format;
}

IOReturn IOAudioStream::setFormatAction(OSObject *owner, void *arg1, void *arg2, void *arg3, void *arg4)
{
    IOReturn result = kIOReturnBadArgument;
    
    if (owner) {
        IOAudioStream *audioStream = OSDynamicCast(IOAudioStream, owner);
        if (audioStream) {
            result = audioStream->setFormat((OSDictionary *)arg1);
        }
    }
    
    return result;
}

IOReturn IOAudioStream::setFormat(const IOAudioStreamFormat *streamFormat, bool callDriver)
{
    IOReturn result = kIOReturnSuccess;
    OSDictionary *formatDict = NULL;
    
    if (streamFormat) {
        if (formatDict = createDictionaryFromFormat(streamFormat)) {
            result = setFormat(streamFormat, formatDict, callDriver);
            formatDict->release();
        } else {
            result = kIOReturnError;
        }
    } else {
        result = kIOReturnBadArgument;
    }

    return result;
}

IOReturn IOAudioStream::setFormat(OSDictionary *formatDict)
{
    IOReturn result = kIOReturnSuccess;

    if (formatDict) {
        IOAudioStreamFormat streamFormat;
        if (createFormatFromDictionary(formatDict, &streamFormat)) {
            result = setFormat(&streamFormat, formatDict);
        } else {
            result = kIOReturnBadArgument;
        }
    } else {
        result = kIOReturnBadArgument;
    }

    return result;
}

IOReturn IOAudioStream::setFormat(const IOAudioStreamFormat *streamFormat, OSDictionary *formatDict, bool callDriver)
{
    IOReturn result = kIOReturnSuccess;
    IOAudioStreamFormat validFormat;
    IOAudioStreamFormatDesc formatDesc;
    
#ifdef DEBUG_CALLS
    IOLog("IOAudioStream[%p]::setFormat(%p, %p)\n", this, streamFormat, formatDict);
#endif

    if (!streamFormat || !formatDict) {
        return kIOReturnBadArgument;
    }
    
#ifdef DEBUG
    setProperty("IOAudioStreamPendingFormat", formatDict);
#endif
    
    validFormat = *streamFormat;
    
    if (validateFormat(&validFormat, &formatDesc)) {
        OSDictionary *sampleRateDict;
        IOAudioSampleRate *newSampleRate = NULL;
        OSSet *userClientsToLock;
        
        sampleRateDict = OSDynamicCast(OSDictionary, formatDict->getObject(kIOAudioSampleRateKey));
        if (sampleRateDict) {
            const IOAudioSampleRate *currentSampleRate;
            
            newSampleRate = IOAudioEngine::createSampleRateFromDictionary(sampleRateDict);
            currentSampleRate = audioEngine->getSampleRate();
            if (newSampleRate && (newSampleRate->whole == currentSampleRate->whole) && (newSampleRate->fraction == currentSampleRate->fraction)) {
                newSampleRate = NULL;
            }
        }
        
        // In order to avoid deadlocks, we need to ensure we hold all of the user client locks
        // before making calls while holding our IO lock.  Everything works fine as long
        // as the order of the locks is workLoop -> user client -> stream.
        // Any other order is sure to cause trouble.
        
        // Because we pause the engine while doing the format change, the user clients will be removed
        // from our list before we complete.  Therefore, we must make a copy of the list to allow
        // all of the clients to be unlocked when we are done.
        userClientsToLock = OSSet::withCapacity(numClients);
        if (userClientsToLock) {
            OSCollectionIterator *clientIterator;
            IOAudioClientBuffer *clientBuf;
            IOAudioEngineUserClient *userClient;
            
            clientBuf = userClientList;
            while (clientBuf) {
                assert(clientBuf->userClient);
                
                userClientsToLock->setObject(clientBuf->userClient);
                clientBuf = clientBuf->nextClient;
            }
        
            clientIterator = OSCollectionIterator::withCollection(userClientsToLock);
            if (!clientIterator) {
                userClientsToLock->release();
                result = kIOReturnNoMemory;
                goto Done;
            }
            
            while (userClient = (IOAudioEngineUserClient *)clientIterator->getNextObject()) {
                userClient->lockBuffers();
            }
            
            clientIterator->release();
        
            lockStreamForIO();
            
            audioEngine->pauseAudioEngine();
            
            if (callDriver) {
                result = audioEngine->performFormatChange(this, &validFormat, newSampleRate);
            }
            
            if (result == kIOReturnSuccess) {
                OSDictionary *newFormatDict;
                
                if (formatDesc.ioFunctionList && (formatDesc.numIOFunctions > 0)) {
                    setIOFunctionList(formatDesc.ioFunctionList, formatDesc.numIOFunctions);
                }
                
                newFormatDict = createDictionaryFromFormat(&validFormat);
                if (newFormatDict) {
                    UInt32 oldNumChannels;
                    
                    if (mixBuffer != NULL) {
                        // If we have a mix buffer and the new format is not mixable, free the mix buffer
                        if (!validFormat.fIsMixable) {
                            setMixBuffer(NULL, 0);
                        } else if (streamAllocatedMixBuffer && (format.fNumChannels != validFormat.fNumChannels)) {	// We need to reallocate the mix buffer
                            UInt32 newMixBufSize;
        
                            assert(audioEngine);
                            newMixBufSize = validFormat.fNumChannels * kIOAudioEngineDefaultMixBufferSampleSize * audioEngine->numSampleFramesPerBuffer;
        
                            if (newMixBufSize > 0) {
                                void *newMixBuf = IOMallocAligned(newMixBufSize, 32);
                                if (newMixBuf) {
                                    setMixBuffer(newMixBuf, newMixBufSize);
                                    streamAllocatedMixBuffer = true;
                                }
                            }
                        }
                    }
                    
                    oldNumChannels = format.fNumChannels;
                    
                    format = validFormat;
                    setProperty(kIOAudioStreamFormatKey, newFormatDict);
                    newFormatDict->release();
    
                    if (format.fNumChannels != oldNumChannels) {
                        audioEngine->updateChannelNumbers();
                    }
    
                    if (newSampleRate) {
                        audioEngine->setSampleRate(newSampleRate);
                    }
                } else {
                    result = kIOReturnError;
                }
            } else {
                IOLog("IOAudioStream<0x%x>::setFormat(0x%x, 0x%x) - audio engine unable to change format\n", (unsigned int)this, (unsigned int)streamFormat, (unsigned int)formatDict);
            }
            
            if (result == kIOReturnSuccess) {
                audioEngine->sendFormatChangeNotification(this);
            }
            
            audioEngine->resumeAudioEngine();
    
            unlockStreamForIO();
            
            // Unlock all of the user clients we originally locked
            assert(userClientsToLock);
            clientIterator = OSCollectionIterator::withCollection(userClientsToLock);
            if (clientIterator) {
                while (userClient = (IOAudioEngineUserClient *)clientIterator->getNextObject()) {
                    userClient->unlockBuffers();
                }
                clientIterator->release();
            } else {
                // Uh oh... we're in trouble now!
                // We have to unlock the clients, but we can't get an iterator on the collection.
                // All existing clients will now hang trying to play audio
                result = kIOReturnNoMemory;
            }
            
            userClientsToLock->release();
        } else {
            result = kIOReturnNoMemory;
        }
    } else {
        IOLog("IOAudioStream<0x%x>::setFormat(0x%x, 0x%x) - invalid format.\n", (unsigned int)this, (unsigned int)streamFormat, (unsigned int)formatDict);
        result = kIOReturnBadArgument;
    }
    
Done:
    
    return result;
}

IOReturn IOAudioStream::hardwareFormatChanged(const IOAudioStreamFormat *streamFormat)
{
    return setFormat(streamFormat, false);
}

void IOAudioStream::addAvailableFormat(const IOAudioStreamFormat *streamFormat, const IOAudioSampleRate *minRate, const IOAudioSampleRate *maxRate, AudioIOFunction ioFunction)
{
    addAvailableFormat(streamFormat, minRate, maxRate, &ioFunction, 1);
}

void IOAudioStream::addAvailableFormat(const IOAudioStreamFormat *streamFormat, const IOAudioSampleRate *minRate, const IOAudioSampleRate *maxRate, const AudioIOFunction *ioFunctionList = NULL, UInt32 numFunctions = 0)
{
    assert(availableFormatDictionaries);

    if (streamFormat && minRate && maxRate) {
        IOAudioStreamFormatDesc *newAvailableFormatList;
        
        newAvailableFormatList = (IOAudioStreamFormatDesc *)IOMallocAligned((numAvailableFormats+1) * sizeof(IOAudioStreamFormatDesc), sizeof (IOAudioStreamFormatDesc *));
        if (newAvailableFormatList) {
            if (availableFormats && (numAvailableFormats > 0)) {
                memcpy(newAvailableFormatList, availableFormats, numAvailableFormats * sizeof(IOAudioStreamFormatDesc));
            }
            newAvailableFormatList[numAvailableFormats].format = *streamFormat;
            newAvailableFormatList[numAvailableFormats].minimumSampleRate = *minRate;
            newAvailableFormatList[numAvailableFormats].maximumSampleRate = *maxRate;
            
            if (ioFunctionList && (numFunctions > 0)) {
                newAvailableFormatList[numAvailableFormats].ioFunctionList = (AudioIOFunction *)IOMallocAligned(numFunctions * sizeof(AudioIOFunction), sizeof (AudioIOFunction *));
                newAvailableFormatList[numAvailableFormats].numIOFunctions = numFunctions;
                memcpy(newAvailableFormatList[numAvailableFormats].ioFunctionList, ioFunctionList, numFunctions * sizeof(AudioIOFunction));
            } else {
                newAvailableFormatList[numAvailableFormats].ioFunctionList = NULL;
                newAvailableFormatList[numAvailableFormats].numIOFunctions = 0;
            }
            
            IOFreeAligned(availableFormats, numAvailableFormats * sizeof(IOAudioStreamFormatDesc));
            availableFormats = newAvailableFormatList;
            numAvailableFormats++;
        }
        
        OSDictionary *formatDict = createDictionaryFromFormat(streamFormat);
        if (formatDict) {
            OSDictionary *sampleRateDict;
        
            sampleRateDict = IOAudioEngine::createDictionaryFromSampleRate(minRate);
            if (sampleRateDict) {
                formatDict->setObject(gMinimumSampleRateKey, sampleRateDict);
                sampleRateDict->release();
                sampleRateDict = IOAudioEngine::createDictionaryFromSampleRate(maxRate);
                if (sampleRateDict) {
                    formatDict->setObject(gMaximumSampleRateKey, sampleRateDict);
                    sampleRateDict->release();
                    availableFormatDictionaries->setObject(formatDict);
                    if (streamFormat->fNumChannels > maxNumChannels) {
                        maxNumChannels = streamFormat->fNumChannels;
                    }
                }
            }
            formatDict->release();
        }
    }
}

void IOAudioStream::clearAvailableFormats()
{
    assert(availableFormatDictionaries);

    availableFormatDictionaries->flushCollection();
}

bool IOAudioStream::validateFormat(IOAudioStreamFormat *streamFormat, IOAudioStreamFormatDesc *formatDesc)
{
    bool foundFormat = false;
    
#ifdef DEBUG_CALLS
    IOLog("IOAudioStream[%p]::validateFormat(%p, %p)\n", this, streamFormat, formatDesc);
#endif
    
    if (streamFormat && availableFormats && (numAvailableFormats > 0)) {
        UInt32 formatIndex;
        
        for (formatIndex = 0; formatIndex < numAvailableFormats; formatIndex++) {
            if ((availableFormats[formatIndex].format.fNumChannels == streamFormat->fNumChannels)
             && (availableFormats[formatIndex].format.fSampleFormat == streamFormat->fSampleFormat)
             && (availableFormats[formatIndex].format.fNumericRepresentation == streamFormat->fNumericRepresentation)
             && (availableFormats[formatIndex].format.fBitDepth == streamFormat->fBitDepth)
             && (availableFormats[formatIndex].format.fBitWidth == streamFormat->fBitWidth)
             && (availableFormats[formatIndex].format.fAlignment == streamFormat->fAlignment)
             && (availableFormats[formatIndex].format.fByteOrder == streamFormat->fByteOrder)) {
                streamFormat->fDriverTag = availableFormats[formatIndex].format.fDriverTag;
                streamFormat->fIsMixable = availableFormats[formatIndex].format.fIsMixable;
                if (formatDesc) {
                    memcpy(formatDesc, &availableFormats[formatIndex], sizeof(IOAudioStreamFormatDesc));
                }
                foundFormat = true;
                break;
            }
        }
    }
    
    return foundFormat;
}

UInt32 IOAudioStream::getStartingChannelID()
{
    return startingChannelID;
}

UInt32 IOAudioStream::getMaxNumChannels()
{
    return maxNumChannels;
}

void IOAudioStream::setStartingChannelNumber(UInt32 channelNumber)
{
    setProperty(kIOAudioStreamStartingChannelNumberKey, channelNumber, sizeof(UInt32)*8);
}

void IOAudioStream::updateNumClients()
{
    setProperty(kIOAudioStreamNumClientsKey, numClients, sizeof(UInt32)*8);
}

IOReturn IOAudioStream::addClient(IOAudioClientBuffer *clientBuffer)
{
    IOReturn result = kIOReturnBadArgument;
#ifdef DEBUG_CALLS
    IOLog("IOAudioStream[%p]::addClient(%p)\n", this, clientBuffer);
#endif

    if (clientBuffer) {
        assert(clientBuffer->audioStream == this);

        lockStreamForIO();
        
        // Make sure this buffer is not in the list
        if ((clientBuffer->nextClip == NULL) && (clientBuffer->previousClip == NULL) && (clientBuffer != clientBufferListStart) && (clientBuffer->nextClient == NULL) && (clientBuffer != userClientList)) {
            
            // It's OK to allow a new client if this is a mixable format
            // or if its not mixable but we don't have any clients
            // or if we are an input stream
            if (format.fIsMixable || (numClients == 0) || (getDirection() == kIOAudioStreamDirectionInput)) {
                numClients++;
                updateNumClients();
                
                clientBuffer->nextClient = userClientList;
                userClientList = clientBuffer;
                
                if (getDirection() == kIOAudioStreamDirectionOutput) {
                
                    clientBuffer->mixedPosition.fLoopCount = 0;
                    clientBuffer->mixedPosition.fSampleFrame = 0;
                    
                    clientBuffer->previousClip = NULL;
                    clientBuffer->nextClip = NULL;
                    
                    if (!mixBuffer && format.fIsMixable && sampleBuffer && (sampleBufferSize > 0)) {
                        assert(audioEngine);
                        
                        UInt32 mixBufSize = format.fNumChannels * kIOAudioEngineDefaultMixBufferSampleSize * audioEngine->numSampleFramesPerBuffer;
                        
                        if (mixBufSize > 0) {
                            void *mixBuf = IOMallocAligned(mixBufSize, 32);
                            if (mixBuf) {
                                setMixBuffer(mixBuf, mixBufSize);
                                streamAllocatedMixBuffer = true;
                            }
                        }
                    }
                }
                
                result = kIOReturnSuccess;
            } else {
                result = kIOReturnExclusiveAccess;
            }
        }
        
        unlockStreamForIO();
    }
    
    return result;
}

void IOAudioStream::removeClient(IOAudioClientBuffer *clientBuffer)
{
#ifdef DEBUG_CALLS
    IOLog("IOAudioStream[%p]::removeClient(%p)\n", this, clientBuffer);
#endif

    if (clientBuffer) {
        IOAudioClientBuffer *tmpClientBuffer, *previousClientBuffer = NULL;
        
        assert(clientBuffer->audioStream == this);
        
        lockStreamForIO();
        
        tmpClientBuffer = userClientList;
        while (tmpClientBuffer && (tmpClientBuffer != clientBuffer)) {
            previousClientBuffer = tmpClientBuffer;
            tmpClientBuffer = tmpClientBuffer->nextClient;
        }
        
        if (tmpClientBuffer) {
            if (previousClientBuffer) {
                previousClientBuffer->nextClient = tmpClientBuffer->nextClient;
            } else {
                assert(tmpClientBuffer == userClientList);
                userClientList = tmpClientBuffer->nextClient;
            }
            
            tmpClientBuffer->nextClient = NULL;

            numClients--;
            updateNumClients();
        }
        
        // Make sure the buffer is in the list
        if ((clientBuffer->nextClip != NULL) || (clientBuffer->previousClip != NULL) || (clientBuffer == clientBufferListStart)) {
            if (getDirection() == kIOAudioStreamDirectionOutput) {
                if (numClients == 0) {
                    resetClipInfo();
                }
                
                if (clientBuffer->previousClip != NULL) {
                    clientBuffer->previousClip->nextClip = clientBuffer->nextClip;
                }
                
                if (clientBuffer->nextClip != NULL) {
                    clientBuffer->nextClip->previousClip = clientBuffer->previousClip;
                }
                
                if (clientBufferListEnd == clientBuffer) {
                    assert(clientBuffer->nextClip == NULL);
                    clientBufferListEnd = clientBuffer->previousClip;
                }
                
                if (clientBufferListStart == clientBuffer) {
                    assert(clientBuffer->previousClip == NULL);
                    clientBufferListStart = clientBuffer->nextClip;
                    if (clientBufferListStart != NULL) {
                        clipIfNecessary();
                    }
                }
            }
        }
		// clear these values for bug 2851917
		clientBuffer->previousClip = NULL;
		clientBuffer->nextClip = NULL;
		clientBuffer->nextClient = NULL;       
		unlockStreamForIO();
    }
}

UInt32 IOAudioStream::getNumClients()
{
    return numClients;
}

void dumpList(IOAudioClientBuffer *start)
{
    IOAudioClientBuffer *tmp;
    
    tmp = start;
    while (tmp) {
        IOLog("(%lx,%lx)\n", tmp->mixedPosition.fLoopCount, tmp->mixedPosition.fSampleFrame);
        tmp = tmp->nextClip;
    }
}

void validateList(IOAudioClientBuffer *start)
{
    IOAudioClientBuffer *tmp;
    
    tmp = start;
    while (tmp) {
        if (tmp->nextClip && (CMP_IOAUDIOENGINEPOSITION(&tmp->mixedPosition, &tmp->nextClip->mixedPosition) > 0)) {
            IOLog("IOAudioStream: ERROR - client buffer list not sorted!\n");
            dumpList(start);
            break;
        }
        tmp = tmp->nextClip;
    }
}

IOReturn IOAudioStream::readInputSamples(IOAudioClientBuffer *clientBuffer, UInt32 firstSampleFrame)
{
    IOReturn result = kIOReturnError;
    
    assert(audioEngine);
    assert(getDirection() == kIOAudioStreamDirectionInput);
    
    if (clientBuffer) {
        UInt32 numWrappedFrames = 0;
        UInt32 numSampleFramesPerBuffer;
        
        numSampleFramesPerBuffer = audioEngine->getNumSampleFramesPerBuffer();
        
        if ((firstSampleFrame + clientBuffer->numSampleFrames) > numSampleFramesPerBuffer) {
            numWrappedFrames = clientBuffer->numSampleFrames - (numSampleFramesPerBuffer - firstSampleFrame);
        }
        
        if (audioIOFunctions && (numIOFunctions != 0)) {
            UInt32 functionNum;
            
            for (functionNum = 0; functionNum < numIOFunctions; functionNum++) {
                if (audioIOFunctions[functionNum]) {
                    result = audioIOFunctions[functionNum](sampleBuffer, clientBuffer->sourceBuffer, firstSampleFrame, clientBuffer->numSampleFrames - numWrappedFrames, &format, this);
                    if (result != kIOReturnSuccess) {
                        break;
                    }
                }
            }
            
            if (numWrappedFrames > 0) {
                for (functionNum = 0; functionNum < numIOFunctions; functionNum++) {
                    if (audioIOFunctions[functionNum]) {
                        result = audioIOFunctions[functionNum](sampleBuffer, &((float *)clientBuffer->sourceBuffer)[(numSampleFramesPerBuffer - firstSampleFrame) * format.fNumChannels], 0, numWrappedFrames, &format, this);
                        if (result != kIOReturnSuccess) {
                            break;
                        }
                    }
                }
            }
        } else {
            result = audioEngine->convertInputSamples(sampleBuffer, clientBuffer->sourceBuffer, firstSampleFrame, clientBuffer->numSampleFrames - numWrappedFrames, &format, this);
            if ((result == kIOReturnSuccess) && (numWrappedFrames > 0)) {
                result = audioEngine->convertInputSamples(sampleBuffer, &((float *)clientBuffer->sourceBuffer)[(numSampleFramesPerBuffer - firstSampleFrame) * format.fNumChannels], 0, numWrappedFrames, &format, this);
            }
        }
    } else {
        result = kIOReturnBadArgument;
    }
    
    return result;
}

IOReturn IOAudioStream::processOutputSamples(IOAudioClientBuffer *clientBuffer, UInt32 firstSampleFrame, UInt32 loopCount, bool samplesAvailable)
{
    IOReturn result = kIOReturnSuccess;
    
#ifdef DEBUG_OUTPUT_CALLS
    //IOLog("IOAudioStream[%p]::processOutputSamples(%p, 0x%lx)\n", this, clientBuffer, firstSampleFrame);
    //IOLog("m(%lx,%lx,%lx)\n", loopCount, firstSampleFrame, clientBuffer->numSampleFrames);
#endif

    assert(direction == kIOAudioStreamDirectionOutput);
    if (clientBuffer) {
        // We can go ahead if we have a mix buffer or if the format is not mixable
        if (mixBuffer || !format.fIsMixable) {
            UInt32 numSampleFramesPerBuffer = audioEngine->getNumSampleFramesPerBuffer();
            UInt32 nextSampleFrame = 0;
            UInt32 mixBufferWrapped = false;
            UInt32 numSamplesToMix = 0;
            IOAudioClientBuffer *tmpBuf = NULL;
                    
            assert(audioEngine);
        
            numSampleFramesPerBuffer = audioEngine->getNumSampleFramesPerBuffer();
            
            // If we haven't mixed any samples for this client yet,
            // we have to figure out which loop those samples belong to
            if (IOAUDIOENGINEPOSITION_IS_ZERO(&clientBuffer->mixedPosition)) {
                clientBuffer->mixedPosition.fSampleFrame = firstSampleFrame;
                clientBuffer->mixedPosition.fLoopCount = loopCount;
            } else {
                // If firstSampleFrame is not the same as the previous mixed position sample frame,
                // then adjust it to the firstSampleFrame - looping if necessary
                if ((clientBuffer->mixedPosition.fSampleFrame != firstSampleFrame) || (clientBuffer->mixedPosition.fLoopCount != loopCount)) {
#ifdef DEBUG
                    IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Mix start position (%lx,%lx) is not previous mixed position (%lx,%lx)\n", this, clientBuffer, loopCount, firstSampleFrame, clientBuffer->mixedPosition.fLoopCount, clientBuffer->mixedPosition.fSampleFrame);
#endif
                    clientBuffer->mixedPosition.fLoopCount = loopCount;
                    clientBuffer->mixedPosition.fSampleFrame = firstSampleFrame;
                }

                // Check to see if the first sample frame is more than one buffer behind the last mixed position
                // of all of the buffers.  We need to deal with the case where we didn't get any samples
                // for this buffer for more than a buffer cycle.  In that case, we need to jump to 
                // the loop that the last buffer is on.  This assumes that a client never gets more than one
                // buffer cycle ahead of the playback head
                if ((clientBuffer != clientBufferListEnd) &&
                    (clientBufferListEnd != NULL) &&
                    ((clientBufferListEnd->mixedPosition.fLoopCount > (clientBuffer->mixedPosition.fLoopCount + 1)) ||
                     ((clientBufferListEnd->mixedPosition.fLoopCount == (clientBuffer->mixedPosition.fLoopCount + 1)) &&
                      (clientBufferListEnd->mixedPosition.fSampleFrame > clientBuffer->mixedPosition.fSampleFrame)))) {
                    // Adjust the loop count to be on the loop before the last mixed position
                    if (clientBuffer->mixedPosition.fSampleFrame > clientBufferListEnd->mixedPosition.fSampleFrame) {
                        clientBuffer->mixedPosition.fLoopCount = clientBufferListEnd->mixedPosition.fLoopCount - 1;
                    } else {
                        clientBuffer->mixedPosition.fLoopCount = clientBufferListEnd->mixedPosition.fLoopCount;
                    }
#ifdef DEBUG
    IOLog("IOAudioStream[%p]::processOutputSamples(%p) - more than one buffer behind (%lx,%lx) adjusting to (%lx,%lx)\n", this, clientBuffer, clientBufferListEnd->mixedPosition.fLoopCount, clientBufferListEnd->mixedPosition.fSampleFrame, clientBuffer->mixedPosition.fLoopCount, firstSampleFrame);
    //dumpList(clientBufferListStart);
#endif
                }
            }
            
            // If we've already clipped, we need to verify all of the samples are after the clipped position
            // Those that are not will be discarded - they can't be played
            if (!IOAUDIOENGINEPOSITION_IS_ZERO(&clippedPosition)) {
                if (clientBuffer->mixedPosition.fLoopCount == clippedPosition.fLoopCount) {
                    if (clientBuffer->mixedPosition.fSampleFrame < clippedPosition.fSampleFrame) {
                        audioEngine->resetClipPosition(this, clientBuffer->mixedPosition.fSampleFrame);

#ifdef DEBUG_OUTPUT
                        UInt32 samplesMissed;
                        samplesMissed = clippedPosition.fSampleFrame - clientBuffer->mixedPosition.fSampleFrame;
                        IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Reset clip position (%lx,%lx)->(%lx,%lx) - %lx samples.\n", this, clientBuffer, clippedPosition.fLoopCount, clippedPosition.fSampleFrame, clientBuffer->mixedPosition.fLoopCount, clientBuffer->mixedPosition.fSampleFrame, samplesMissed);
#endif

                        clippedPosition = clientBuffer->mixedPosition;
                    }
                } else if (clientBuffer->mixedPosition.fLoopCount < clippedPosition.fLoopCount) {
                    audioEngine->resetClipPosition(this, clientBuffer->mixedPosition.fSampleFrame);
                    
#ifdef DEBUG_OUTPUT
                    UInt32 samplesMissed;
                    samplesMissed = (clippedPosition.fLoopCount - clientBuffer->mixedPosition.fLoopCount - 1) * numSampleFramesPerBuffer;
                    samplesMissed += clippedPosition.fSampleFrame + numSampleFramesPerBuffer - clientBuffer->mixedPosition.fSampleFrame;
                    IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Reset clip position (%lx,%lx)->(%lx,%lx) - %lx samples.\n", this, clientBuffer, clippedPosition.fLoopCount, clippedPosition.fSampleFrame, clientBuffer->mixedPosition.fLoopCount, clientBuffer->mixedPosition.fSampleFrame, samplesMissed);
#endif

                    clippedPosition = clientBuffer->mixedPosition;
                }
            }
            
            // We only need to mix samples if there are samples available
            // If the watchdog timer was responsible for this call, then 
            // there won't be any samples, so there's no point in mixing
            // or resetting the clip position
            if (samplesAvailable) {
                numSamplesToMix = clientBuffer->numSampleFrames;
            }
            
            if (numSamplesToMix > 0) {
/*
#ifdef DEBUG_OUTPUT
                UInt32 currentSampleFrame = audioEngine->getCurrentSampleFrame();
                if (currentSampleFrame > firstSampleFrame) {
                    if ((firstSampleFrame + clientBuffer->numSampleFrames) > currentSampleFrame) {
                        //IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Error: Some samples already played: first=%lx num=%lx curr=%lx\n", this, clientBuffer, firstSampleFrame, clientBuffer->numSampleFrames, currentSampleFrame);
                        IOLog("mix() missed first=%lx num=%lx curr=%lx\n", firstSampleFrame, clientBuffer->numSampleFrames, currentSampleFrame);
                    }
                } else {
                    if ((clientBuffer->numSampleFrames + firstSampleFrame) > (currentSampleFrame + numSampleFramesPerBuffer)) {
                        //IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Error: Some samples already played: first=%lx num=%lx curr=%lx\n", this, clientBuffer, firstSampleFrame, clientBuffer->numSampleFrames, currentSampleFrame);
                        IOLog("mix() missed first=%lx num=%lx curr=%lx\n", firstSampleFrame, clientBuffer->numSampleFrames, currentSampleFrame);
                    }
                }
#endif
*/
                // Only mix output samples if this is a mixable format
                if (format.fIsMixable) {
                    // Check if the buffer wraps
                    if (numSampleFramesPerBuffer > (firstSampleFrame + numSamplesToMix)) {	// No wrap
                        result = audioEngine->mixOutputSamples(clientBuffer->sourceBuffer, mixBuffer, firstSampleFrame, numSamplesToMix, &format, this);
                        nextSampleFrame = firstSampleFrame + numSamplesToMix;
                    } else {	// Buffer wraps around
                        mixBufferWrapped = true;
                        result = audioEngine->mixOutputSamples(clientBuffer->sourceBuffer, mixBuffer, firstSampleFrame, numSampleFramesPerBuffer - firstSampleFrame, &format, this);
                        if (result != kIOReturnSuccess) {
                            IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Error: 0x%x returned from audioEngine->mixOutputSamples(%p, %p, 0x%lx, 0x%lx, %p, %p)\n", this, clientBuffer, result, clientBuffer->sourceBuffer, mixBuffer, firstSampleFrame, numSampleFramesPerBuffer - firstSampleFrame, &format, this);
                        }
                        nextSampleFrame = numSamplesToMix - (numSampleFramesPerBuffer - firstSampleFrame);
                        result = audioEngine->mixOutputSamples(((float *)clientBuffer->sourceBuffer) + ((numSampleFramesPerBuffer - firstSampleFrame) * format.fNumChannels), mixBuffer, 0, nextSampleFrame, &format, this);
                    }
                }
            
                if (result == kIOReturnSuccess) {
                    // Reset startingSampleFrame and startingLoopCount if we haven't clipped
                    // anything yet and this buffer mixed samples before the previous
                    // starting frame
                    if (IOAUDIOENGINEPOSITION_IS_ZERO(&clippedPosition)) {
                        if (IOAUDIOENGINEPOSITION_IS_ZERO(&startingPosition) ||
                            (clientBuffer->mixedPosition.fLoopCount < startingPosition.fLoopCount) ||
                            ((clientBuffer->mixedPosition.fLoopCount == startingPosition.fLoopCount) && (firstSampleFrame < startingPosition.fSampleFrame))) {
                            
                            startingPosition.fLoopCount = clientBuffer->mixedPosition.fLoopCount;
                            startingPosition.fSampleFrame = firstSampleFrame;
                        }
                    }
                } else {
                    IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Error: 0x%x returned from audioEngine->mixOutputSamples(%p, %p, 0x%lx, 0x%lx, %p, %p)\n", this, clientBuffer, result, clientBuffer->sourceBuffer, mixBuffer, firstSampleFrame, numSampleFramesPerBuffer - firstSampleFrame, &format, this);
                }
                
                if (mixBufferWrapped) {
                    clientBuffer->mixedPosition.fLoopCount++;
                }
                clientBuffer->mixedPosition.fSampleFrame = nextSampleFrame;
                
            } else {	// We missed all of the samples
                clientBuffer->mixedPosition.fSampleFrame += clientBuffer->numSampleFrames;
                if (clientBuffer->mixedPosition.fSampleFrame >= numSampleFramesPerBuffer) {
                    clientBuffer->mixedPosition.fSampleFrame -= numSampleFramesPerBuffer;
                    clientBuffer->mixedPosition.fLoopCount++;
                }
            }
            
            // If this buffer isn't in the list yet, then we look at the beginning of the list
            if ((clientBuffer->nextClip == NULL) && (clientBuffer->previousClip == NULL) && (clientBuffer != clientBufferListStart)) {
                // If the buffer has mixed past the first buffer in the list, then we can start at the beginning
                // If not, then tmpBuf is just NULL and we insert at the beginning
                if ((clientBufferListStart != NULL) && (CMP_IOAUDIOENGINEPOSITION(&clientBuffer->mixedPosition, &clientBufferListStart->mixedPosition) > 0)) {
                    tmpBuf = clientBufferListStart;
                }
            } else { // Otherwise, we look forward from the current position
                tmpBuf = clientBuffer;
            }
                
            // Add it to the beginning if the buffer is new and has not mixed past any other buffers
            if (tmpBuf == NULL) {
                assert(clientBuffer->nextClip == NULL);
                assert(clientBuffer->previousClip == NULL);
                
                clientBuffer->nextClip = clientBufferListStart;
                clientBufferListStart = clientBuffer;
                
                if (clientBuffer->nextClip == NULL) {
                    clientBufferListEnd = clientBuffer;
                } else {
                    clientBuffer->nextClip->previousClip = clientBuffer;
                }
            } else {
                //Find the insertion point for the new location for this buffer
                while ((tmpBuf->nextClip != NULL) && (CMP_IOAUDIOENGINEPOSITION(&clientBuffer->mixedPosition, &tmpBuf->nextClip->mixedPosition) > 0)) {
                    tmpBuf = tmpBuf->nextClip;
                }
            
                if (tmpBuf != clientBuffer) {
                    // If the buffer is to change position, move updated client buffer to its new sorted position
                    // First remove the client buffer from its current position
                    if (clientBuffer->previousClip != NULL) {
                        clientBuffer->previousClip->nextClip = clientBuffer->nextClip;
                    } else if (clientBuffer == clientBufferListStart) {	// If we don't have a previous clip set, we may be the starting entry
                        clientBufferListStart = clientBuffer->nextClip;
                    } // If we have don't have a previousClip set and are not the start, then this is the first time this buffer is being mixed
                    
                    if (clientBuffer->nextClip != NULL) {
                        clientBuffer->nextClip->previousClip = clientBuffer->previousClip;
                    } else if (clientBuffer == clientBufferListEnd) {	// If we don't have a next clip set, we may be the last entry
                        // We should never get here, because we only are moving this buffer forward
                        // and that is impossible if it is the last one
                        clientBufferListEnd = clientBuffer->previousClip;
                    } // If we don't have a next clip and are not the end, then this is the first time this buffer is being mixed
                    
                    // Insert it after tmpBuf
                    clientBuffer->nextClip = tmpBuf->nextClip;
                    clientBuffer->previousClip= tmpBuf;
                    tmpBuf->nextClip = clientBuffer;
                    if (clientBuffer->nextClip) {
                        clientBuffer->nextClip->previousClip = clientBuffer;
                    }
                    if (clientBuffer->nextClip == NULL) {
                        assert(clientBufferListEnd == tmpBuf);
                        clientBufferListEnd = clientBuffer;
                    }
                    
#ifdef DEBUG
                    validateList(clientBufferListStart);
#endif
                }
            }
            
            // We should attempt to clip if we mixed some samples of if we
            // were called as a result of the watchdog timer (indicated
            // by samplesAvailable being false)
            if ((numSamplesToMix > 0) || !samplesAvailable) {
                clipIfNecessary();
            }
        } else {
            IOLog("IOAudioStream[%p]::processOutputSamples(%p) - Internal Error: No mix buffer\n", this, clientBuffer);
            result = kIOReturnError;
        }
    } else {
        result = kIOReturnBadArgument;
    }
    
    return result;
}

void IOAudioStream::resetClipInfo()
{
    startingPosition.fLoopCount = 0;
    startingPosition.fSampleFrame = 0;
    clippedPosition.fLoopCount = 0;
    clippedPosition.fSampleFrame = 0;
}

void IOAudioStream::clipIfNecessary()
{
#ifdef DEBUG_OUTPUT_CALLS
    //IOLog("IOAudioStream[%p]::clipIfNecessary()\n", this);
#endif

    if (clientBufferListStart != NULL) {
        // Only try to clip if there is not an unmixed buffer
        if (!IOAUDIOENGINEPOSITION_IS_ZERO(&clientBufferListStart->mixedPosition)) {
        
            // Check to see if we've clipped any samples yet
            if (IOAUDIOENGINEPOSITION_IS_ZERO(&clippedPosition)) {
                clippedPosition = startingPosition;
            }
            
#ifdef DEBUG_OUTPUT
            IOAudioClientBuffer *tmp;
            
            tmp = clientBufferListStart->nextClip;
            while (tmp) {
                if ((tmp->mixedPosition.fLoopCount > (clippedPosition.fLoopCount + 1)) ||
                    ((tmp->mixedPosition.fLoopCount == clippedPosition.fLoopCount) && 
                     (tmp->mixedPosition.fSampleFrame > clippedPosition.fSampleFrame))) {
                     
                    if (clientBufferListStart->mixedPosition.fSampleFrame > clippedPosition.fSampleFrame) {
                        if ((tmp->mixedPosition.fSampleFrame > clippedPosition.fSampleFrame) &&
                            (clientBufferListStart->mixedPosition.fSampleFrame > tmp->mixedPosition.fSampleFrame)) {
                            IOLog("IOAudioStream[%p]::clipIfNecessary() - Error: Clipping across future buffer boundary - glitching! (%lx,%lx)->(%lx,%lx) buf=(%lx,%lx)\n", this, clippedPosition.fLoopCount, clippedPosition.fSampleFrame, clientBufferListStart->mixedPosition.fLoopCount, clientBufferListStart->mixedPosition.fSampleFrame, tmp->mixedPosition.fLoopCount, tmp->mixedPosition.fSampleFrame);
                            dumpList(clientBufferListStart);
                            break;
                        }
                    } else if (clippedPosition.fSampleFrame > clientBufferListStart->mixedPosition.fSampleFrame) {
                        if ((tmp->mixedPosition.fSampleFrame < clientBufferListStart->mixedPosition.fSampleFrame) ||
                            (tmp->mixedPosition.fSampleFrame > clippedPosition.fSampleFrame)) {
                            IOLog("IOAudioStream[%p]::clipIfNecessary() - Error: Clipping across future buffer boundary - glitching! (%lx,%lx)->(%lx,%lx) buf=(%lx,%lx)\n", this, clippedPosition.fLoopCount, clippedPosition.fSampleFrame, clientBufferListStart->mixedPosition.fLoopCount, clientBufferListStart->mixedPosition.fSampleFrame, tmp->mixedPosition.fLoopCount, tmp->mixedPosition.fSampleFrame);
                            dumpList(clientBufferListStart);
                            break;
                        }
                    }
                }
                tmp = tmp->nextClip;
            }
#endif
            
            // Check to see if it is on the same loop as the starting position
            // If not, adjust it to the same loop
            if (((clientBufferListStart->mixedPosition.fLoopCount == (clippedPosition.fLoopCount + 1)) &&
                 (clientBufferListStart->mixedPosition.fSampleFrame >= clippedPosition.fSampleFrame)) ||
                (clientBufferListStart->mixedPosition.fLoopCount > (clippedPosition.fLoopCount + 1))) {
                IOLog("IOAudioStream[%p]::clipIfNecessary() - Error: attempting to clip to a position more than one buffer ahead of last clip position (%lx,%lx)->(%lx,%lx).\n", this, clippedPosition.fLoopCount, clippedPosition.fSampleFrame, clientBufferListStart->mixedPosition.fLoopCount, clientBufferListStart->mixedPosition.fSampleFrame);
                if (clientBufferListStart->mixedPosition.fSampleFrame >= clippedPosition.fSampleFrame) {
                    clippedPosition.fLoopCount = clientBufferListStart->mixedPosition.fLoopCount;
                } else {
                    clippedPosition.fLoopCount = clientBufferListStart->mixedPosition.fLoopCount - 1;
                }
                IOLog("IOAudioStream[%p]::clipIfNecessary() - adjusting clipped position to (%lx,%lx)\n", this, clippedPosition.fLoopCount, clippedPosition.fSampleFrame);
            }
            
            if (clientBufferListStart->mixedPosition.fLoopCount == clippedPosition.fLoopCount) {
                if (clientBufferListStart->mixedPosition.fSampleFrame > clippedPosition.fSampleFrame) {
                    clipOutputSamples(clippedPosition.fSampleFrame, clientBufferListStart->mixedPosition.fSampleFrame - clippedPosition.fSampleFrame);
                    clippedPosition.fSampleFrame = clientBufferListStart->mixedPosition.fSampleFrame;
                } else if (clientBufferListStart->mixedPosition.fSampleFrame < clippedPosition.fSampleFrame) {
                    //!!FIXME!! Put the log back in
                    IOLog("IOAudioStream[%p]::clipIfNecessary() - Error: already clipped to a position (0x%lx,0x%lx) past data to be clipped (0x%lx, 0x%lx) - data ignored.\n", this, clippedPosition.fLoopCount, clippedPosition.fSampleFrame, clientBufferListStart->mixedPosition.fLoopCount, clientBufferListStart->mixedPosition.fSampleFrame);
                    //clippedPosition.fSampleFrame = clientBufferListStart->mixedPosition.fSampleFrame;
                }
            } else {	// Clip wraps around
                UInt32 numSampleFramesPerBuffer;
                
                assert(audioEngine);
                
                numSampleFramesPerBuffer = audioEngine->getNumSampleFramesPerBuffer();
                
                clipOutputSamples(clippedPosition.fSampleFrame, numSampleFramesPerBuffer - clippedPosition.fSampleFrame);
                clipOutputSamples(0, clientBufferListStart->mixedPosition.fSampleFrame);
                
                clippedPosition = clientBufferListStart->mixedPosition;
            }
        }
    }
}

void IOAudioStream::clipOutputSamples(UInt32 firstSampleFrame, UInt32 numSampleFrames)
{
    IOReturn result = kIOReturnSuccess;
    
#ifdef DEBUG_OUTPUT_CALLS
    //IOLog("IOAudioStream[%p]::clipOutputSamples(0x%lx, 0x%lx)\n", this, firstSampleFrame, numSampleFrames);
    //IOLog("c(%lx,%lx) %lx\n", firstSampleFrame, numSampleFrames, audioEngine->getCurrentSampleFrame());
#endif

    assert(direction == kIOAudioStreamDirectionOutput);
    assert(audioEngine);
    
    if (!mixBuffer || !sampleBuffer) {
        IOLog("IOAudioStream[%p]::clipOutputSamples(0x%lx, 0x%lx) - Internal Error: mixBuffer = %p - sampleBuffer = %p\n", this, firstSampleFrame, numSampleFrames, mixBuffer, sampleBuffer);
        return;
    }
    
/*
#ifdef DEBUG_OUTPUT
    UInt32 currentSampleFrame = audioEngine->getCurrentSampleFrame();
    
    if (currentSampleFrame > firstSampleFrame) {
        if ((firstSampleFrame + numSampleFrames) > currentSampleFrame) {
            //IOLog("IOAudioStream[%p]::clipOutputSamples(0x%lx, 0x%lx) - too late for some samples - current position = 0x%lx.\n", this, firstSampleFrame, numSampleFrames, currentSampleFrame);
            IOLog("clip(%lx,%lx) missed curr=%lx.\n", firstSampleFrame, numSampleFrames, currentSampleFrame);
        }
    } else {
        if ((numSampleFrames + firstSampleFrame) > (currentSampleFrame + audioEngine->getNumSampleFramesPerBuffer())) {
            //IOLog("IOAudioStream[%p]::clipOutputSamples(0x%lx, 0x%lx) - too late for some samples - current position = 0x%lx.\n", this, firstSampleFrame, numSampleFrames, currentSampleFrame);
            IOLog("clip(%lx,%lx) missed curr=%lx.\n", firstSampleFrame, numSampleFrames, currentSampleFrame);
        }
    }
#endif
*/
    
    if (audioIOFunctions && (numIOFunctions != 0)) {
        UInt32 functionNum;
        
        for (functionNum = 0; functionNum < numIOFunctions; functionNum++) {
            if (audioIOFunctions[functionNum]) {
                result = audioIOFunctions[functionNum](mixBuffer, sampleBuffer, firstSampleFrame, numSampleFrames, &format, this);
                if (result != kIOReturnSuccess) {
                    break;
                }
            }
        }
    } else {
        result = audioEngine->clipOutputSamples(mixBuffer, sampleBuffer, firstSampleFrame, numSampleFrames, &format, this);
    }
    
    if (result != kIOReturnSuccess) {
        IOLog("IOAudioStream[%p]::clipOutputSamples(0x%lx, 0x%lx) - clipping function returned error: 0x%x\n", this, firstSampleFrame, numSampleFrames, result);
    }
}

void IOAudioStream::lockStreamForIO()
{
    assert(streamIOLock);
    
    IORecursiveLockLock(streamIOLock);
}

void IOAudioStream::unlockStreamForIO()
{
    assert(streamIOLock);
    
    IORecursiveLockUnlock(streamIOLock);
}

void IOAudioStream::setStreamAvailable(bool available)
{
    if (streamAvailable != available) {
        streamAvailable = available;
        setProperty(kIOAudioStreamAvailableKey, available ? 1 : 0, sizeof(UInt8)*8);
        
        assert(audioEngine);
        audioEngine->updateChannelNumbers();
    }
}

bool IOAudioStream::getStreamAvailable()
{
    return streamAvailable;
}

IOReturn IOAudioStream::addDefaultAudioControl(IOAudioControl *defaultAudioControl)
{
    IOReturn result = kIOReturnBadArgument;
    
    if (defaultAudioControl) {
        UInt32 controlChannelID;
        
        if (defaultAudioControl->getChannelID() == kIOAudioControlChannelIDAll) {
            if (((getDirection() == kIOAudioStreamDirectionOutput) && (defaultAudioControl->getUsage() == kIOAudioControlUsageInput)) ||
                ((getDirection() == kIOAudioStreamDirectionInput) && (defaultAudioControl->getUsage() == kIOAudioControlUsageOutput))) {
                result = kIOReturnError;
                IOLog("IOAudioStream[%p]::addDefaultAudioControl() - Error: invalid audio control - stream direction is opposite of control usage.\n", this);
                goto Done;
            }
            
            controlChannelID = defaultAudioControl->getChannelID();
            
            if ((controlChannelID != 0) && ((controlChannelID < startingChannelID) || (controlChannelID >= (startingChannelID + maxNumChannels)))) {
                result = kIOReturnError;
                IOLog("IOAudioStream[%p]::addDefaultAudioControl() - Error: audio control channel is not in this stream.\n", this);
                goto Done;
            }
            
            if (defaultAudioControl->attachAndStart(this)) {
                if (!defaultAudioControls) {
                    defaultAudioControls = OSSet::withObjects(&(const OSObject *)defaultAudioControl, 1, 1);
                } else {
                    defaultAudioControls->setObject(defaultAudioControl);
                }
            } else {
                result = kIOReturnError;
            }
        } else {	// Control for an individual channel - attach to audio engine instead
            assert(audioEngine);
            result = audioEngine->addDefaultAudioControl(defaultAudioControl);
        }
    }
   
Done:

    return result;
}

void IOAudioStream::removeDefaultAudioControls()
{
    if (defaultAudioControls) {
        if (!isInactive()) {
            OSCollectionIterator *controlIterator;
            
            controlIterator = OSCollectionIterator::withCollection(defaultAudioControls);
            
            if (controlIterator) {
                IOAudioControl *control;
                
                while (control = (IOAudioControl *)controlIterator->getNextObject()) {
                    if (control->getProvider() == this) {
                        control->terminate();
                    } else {
                        control->detach(this);
                    }
                }
                
                controlIterator->release();
            }
        }
        
        defaultAudioControls->flushCollection();
    }
}
