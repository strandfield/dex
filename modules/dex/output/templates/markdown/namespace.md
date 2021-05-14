
{% if namespace.name.length == 0 %}
# Global namespace
{% else %}
# {{ namespace.name }} namespace
{% endif %}

{% if namespace.brief or namespace.description %}

{% if namespace.brief %}
**Brief:** {{ namespace.brief }}
{% endif %}

## Detailed description

{{ namespace.description }}

{% endif %}

## Functions

{% for f in namespace.entities | filter_by_type: 'function' %}

### {{ f | funsig }}

{% if f.brief %}
**Brief:** {{ f.brief }}
{% endif %}

{% if f.parameters and f.parameters.size > 0 %}
Parameters:
{% for p in f.parameters %}
- {{ p.brief }}
{% endfor %}
{% endif %}

{% if f.returns %}
**Returns:** {{ f.returns }}
{% endif %}

{{ f.description }}

{% endfor %}

