//
//  OfxHost.h
//  Natron
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */
//  Created by Frédéric Devernay on 03/09/13.
//
//

#ifndef NATRON_ENGINE_OFXHOST_H_
#define NATRON_ENGINE_OFXHOST_H_

#include <QtCore/QStringList>
#include <QtCore/QObject>
#include <QtCore/QString>

#include <ofxhPluginCache.h>
#include <ofxhImageEffectAPI.h>

#include "Global/Macros.h"

class OfxEffectInstance;
class AppInstance;
class QMutex;
namespace Natron {
class Node;
class Plugin;
class OfxHost : public QObject,public OFX::Host::ImageEffect::Host {
    
    Q_OBJECT
    
public:
    
    OfxHost();
    
    virtual ~OfxHost();

    /// Create a new instance of an image effect plug-in.
    ///
    /// It is called by ImageEffectPlugin::createInstance which the
    /// client code calls when it wants to make a new instance.
    ///
    ///   \arg clientData - the clientData passed into the ImageEffectPlugin::createInstance
    ///   \arg plugin - the plugin being created
    ///   \arg desc - the descriptor for that plugin
    ///   \arg context - the context to be created in
    virtual OFX::Host::ImageEffect::Instance* newInstance(void* clientData,
                                                          OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                                          OFX::Host::ImageEffect::Descriptor& desc,
                                                          const std::string& context) OVERRIDE;

    /// Override this to create a descriptor, this makes the 'root' descriptor
    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(OFX::Host::ImageEffect::ImageEffectPlugin* plugin) OVERRIDE;

    /// used to construct a context description, rootContext is the main context
    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(const OFX::Host::ImageEffect::Descriptor &rootContext,
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plug) OVERRIDE;

    /// used to construct populate the cache
    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(const std::string &bundlePath,
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plug) OVERRIDE;

    /// vmessage
    virtual OfxStatus vmessage(const char* type,
                               const char* id,
                               const char* format,
                               va_list args) OVERRIDE;

    /// setPersistentMessage
    virtual OfxStatus setPersistentMessage(const char* type,
                                           const char* id,
                                           const char* format,
                                           va_list args) OVERRIDE;
    /// clearPersistentMessage
    virtual OfxStatus clearPersistentMessage() OVERRIDE;

    virtual void loadingStatus(const std::string &) OVERRIDE;
    
    ///fetch the parametric parameters suite or returns the base class version
    virtual void* fetchSuite(const char *suiteName, int suiteVersion) OVERRIDE;
    
    virtual bool implementsMultiThreadSuite() const OVERRIDE ;
    
    virtual OfxStatus multiThread(OfxThreadFunctionV1 func,unsigned int nThreads, void *customArg) OVERRIDE;

    virtual void multiThreadNumCPUS(unsigned int *nCPUs) const OVERRIDE;
    
    virtual void multiThreadIndex(unsigned int *threadIndex) const OVERRIDE;
    
    virtual bool multiThreadIsSpawnedThread() const OVERRIDE;
    
    virtual void mutexCreate(OfxMutexHandle *mutex, int lockCount) const OVERRIDE;

    virtual void mutexDestroy(const OfxMutexHandle mutex) const OVERRIDE;

    virtual void mutexLock(const OfxMutexHandle mutex) const OVERRIDE;
   
    virtual void mutexUnLock(const OfxMutexHandle mutex) const OVERRIDE;

    virtual OfxStatus mutexTryLock(const OfxMutexHandle mutex) const OVERRIDE;
    
    OfxEffectInstance* createOfxEffect(const std::string& name,Node* node);

    /*Reads OFX plugin cache and scan plugins directories
     to load them all.*/
    void loadOFXPlugins(std::vector<Natron::Plugin *> *plugins);

signals:
    void toolButtonAdded(QStringList,QString,QString,QString,QString);
    
private:
    /*Writes all plugins loaded and their descriptors to
     the OFX plugin cache. (called by the destructor) */
    void writeOFXCache();
    
    OFX::Host::ImageEffect::PluginCache _imageEffectPluginCache;


    /*plugin name -> pair< plugin id , plugin grouping >
     The name of the plugin is followed by the first part of the grouping in brackets
     to help identify two distinct plugins with the same name. e.g :
     1)Invert [OFX]  with plugin id net.sourceforge.openfx.invert and grouping OFX/
     2)Invert [Toto] with plugin id com.toto.invert and grouping Toto/SuperPlugins/OFX/
     */
    struct OFXPluginEntry {
        std::string openfxId;
        std::string grouping;
        OFXPluginEntry(const std::string &ofxid, const std::string &grp)
        : openfxId(ofxid)
        , grouping(grp)
        {}
        OFXPluginEntry() {}
    };
    typedef std::map<std::string,OFXPluginEntry> OFXPluginsMap;
    typedef OFXPluginsMap::const_iterator OFXPluginsIterator;

    OFXPluginsMap _ofxPlugins;
};

} // namespace Natron

#endif
