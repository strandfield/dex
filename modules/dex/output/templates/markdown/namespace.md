
{% if namespace.name.length == 0 %}
# Global namespace
{% else %}
# {{ namespace.name }} namespace
{% endif %}

{% if namespace.documentation %}

{% if namespace.documentation.brief %}
**Brief:** {{ namespace.documentation.brief }}
{% endif %}

## Detailed description

{{ namespace.documentation.description }}

{% endif %}

## Functions

{% for f in namespace.entities | filter_by_type: 'function' %}

### {{ f | funsig }}

{% if f.documentation.brief %}
**Brief:** {{ f.documentation.brief }}
{% endif %}

{% if f.parameters and f.parameters.size > 0 %}
Parameters:
{% for p in f.parameters %}
- {{ p.documentation }}
{% endfor %}
{% endif %}

{% if f.documentation.returns %}
**Returns:** {{ f.documentation.returns }}
{% endif %}

{{ f.documentation.description }}

{% endfor %}

