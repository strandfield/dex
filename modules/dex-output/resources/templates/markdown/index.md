
# Documentation

Welcome !

{% assign classes = model.program.global_namespace.entities | filter_by_type: 'class' %}

{% if classes and classes.length > 0 %}
## Classes

{% for c in classes %}
- [{{ c.name }}]({{c.url}})
{% endfor %}

{% endif %}
