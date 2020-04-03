#pragma once
#include <QDomElement>

#include "effects/defs.h"
#include "effects/effectmanifest.h"
#include "effects/presets/effectparameterpreset.h"

class EffectPreset {
  public:
    EffectPreset();
    EffectPreset(const QDomElement& element);
    EffectPreset(const EffectSlotPointer pEffectSlot);
    EffectPreset(const EffectManifestPointer pManifest);
    ~EffectPreset();

    const QDomElement toXml(QDomDocument* doc) const;

    const QString& id() const {
        return m_id;
    }

    bool isNull() const {
        return m_id.isEmpty();
    }

    EffectBackendType backendType() const {
        return m_backendType;
    }

    double metaParameter() const {
        return m_dMetaParameter;
    }

    const QList<EffectParameterPreset>& getParameterPresets() const {
        return m_effectParameterPresets;
    }

  private:
    QString m_id;
    EffectBackendType m_backendType;
    double m_dMetaParameter;

    QList<EffectParameterPreset> m_effectParameterPresets;
};
